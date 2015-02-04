#include "dbfsdriver.h"
#include "connection.h"
#include "query.h"
#include "tablelocker.h"
#include "../model/sqltablemodel.h"

#include <QSqlDatabase>
#include <QSqlError>
#include <QDir>
#include <QStringBuilder>

#define sqlDebug qfInfo

using namespace qf::core::sql;

static const QString COL_ID("id");
static const QString COL_INODE("inode");
static const QString COL_PINODE("pinode");
static const QString COL_TYPE("type");
static const QString COL_NAME("name");
static const QString COL_META("meta");
static const QString COL_SNAPSHOT("snapshot");
static const QString COL_MTIME("mtime");
static const QString COL_DELETED("deleted");
static const QString COL_COPIED_IN_SNAPSHOT("copiedInSnapshot");
static const QString COL_DATA("data");
static const QString COL_SIZE("size");

static const QString COL_SS_NO("no");
static const QString COL_SS_TS("ts");
static const QString COL_SS_COMMENT("comment");

static const QString O_LOCK_EXCLUSIVE = QStringLiteral("EXCLUSIVE");
static const bool O_CREATE = true;
static const bool O_DELETE = true;
static const bool O_POST_NOTIFY = true;

static const QString CHANNEL_INVALIDATE_DBFSDRIVER_CACHE = "invalidateDbFsDriverCache";

DbFsDriver::DbFsDriver(QObject *parent)
	: QObject(parent)
{
	m_connectionName = QLatin1String(QSqlDatabase::defaultConnection);
	m_tableName = QStringLiteral("dbfs");
}

DbFsDriver::~DbFsDriver()
{
}

DbFsAttrs DbFsDriver::attributes(const QString &path)
{
	//qfLogFuncFrame() << path;
	QString spath = cleanPath(path);
	if(!m_attributeCache.contains(spath)) {
		DbFsAttrs a = readAttrs(spath);
		m_attributeCache[spath] = a;
	}
	DbFsAttrs ret = m_attributeCache.value(spath);
	//qfDebug() << ret.toString();
	return ret;
}

QList<DbFsAttrs> DbFsDriver::childAttributes(const QString &parent_path)
{
	qfLogFuncFrame() << parent_path;
	QString clean_ppath = cleanPath(parent_path);
	qfDebug() << "\t cleaned path:" << clean_ppath;
	QList<DbFsAttrs> ret;
	if(m_directoryCache.contains(clean_ppath)) {
		for(QString entry : m_directoryCache.value(clean_ppath)) {
			QString p = joinPath(clean_ppath, entry);
			ret << attributes(p);
		}
	}
	else {
		QStringList dir_entry_list;
		DbFsAttrs parent_attrs = attributes(clean_ppath);
		if(!parent_attrs.isNull() && parent_attrs.type() == DbFsAttrs::Dir) {
			int parent_inode = parent_attrs.inode();
			ret = readChildAttrs(parent_inode);
			for(auto attrs : ret) {
				QString path = joinPath(clean_ppath, attrs.name());
				//qfDebug() << clean_ppath + "name:" << attrs.name() << "->" << path;
				m_attributeCache[path] = attrs;
				dir_entry_list << attrs.name();
			}
		}
		else {
			qfWarning() << "Node on path:" << parent_path << "is not dir:" << parent_attrs.toString();
		}
		m_directoryCache[clean_ppath] = dir_entry_list;
	}
	return ret;
}

Connection DbFsDriver::connection()
{
	QSqlDatabase db = QSqlDatabase::database(connectionName(), false);
	QF_ASSERT_EX(db.isOpen(), tr("Connection '%1' is not open!").arg(connectionName()));
	/*
	if(!m_isNotifyRegistered) {
		auto drv = db.driver();
		bool ok = drv->subscribeToNotification(CHANNEL_INVALIDATE_DBFSDRIVER_CACHE);
		if(!ok) {
			qfError() << "Cannot subscribe channel" << CHANNEL_INVALIDATE_DBFSDRIVER_CACHE << "for notification";
		}
		else {
			connect(drv, SIGNAL(notification(QString,QSqlDriver::NotificationSource,QVariant)), this, SLOT(onSqlNotify(QString,QSqlDriver::NotificationSource,QVariant)));
		}
		m_isNotifyRegistered = true;
	}
	*/
	return Connection(db);
}

QString DbFsDriver::snapshotsTableName()
{
	return tableName() + "_snapshots";
}

bool DbFsDriver::checkWritePermissions()
{
	bool ok = !isSnapshotReadOnly();
	return ok;
}

DbFsAttrs DbFsDriver::touch(const DbFsAttrs &attrs, bool create_node, bool delete_node, const QByteArray &data)
{
	qfLogFuncFrame() << attrs.toString() << "create:" << create_node << "delete:" << delete_node;
	DbFsAttrs ret;
	int latest_sn = latestSnapshotNumber();
	if(!create_node) {
		if(attrs.snapshot() < latestSnapshotNumber()) {
			/// node exists in previous snapshot
			if(delete_node) {
				if(!attrs.isDeleted()) {
					/// create empty node to mark it deleted in current snapshot
					ret = attrs;
					ret.setDeleted(true);
					ret.setSnapshot(latest_sn);
					ret = sqlInsertNode(ret, QByteArray());
				}
				else {
					qfWarning() << "Node is deleted already:" << attrs.toString();
					return DbFsAttrs();
				}
			}
			else {
				/// create copy of node for latest snapshot
				ret = attrs;
				ret.setSnapshot(latest_sn);
				ret = sqlInsertNode(ret, data);
			}
		}
		else {
			/// existing node modified in current version
			if(delete_node) {
				/// delete existing not versioned node
				ret = sqlDeleteNode(attrs);
			}
			else {
				/// change mtime and data
				ret = attrs;
				ret = sqlUpdateNode(ret, data);
			}
		}
	}
	else {
		/// create brand new node
		ret = attrs;
		ret.setSnapshot(latest_sn);
		ret = sqlInsertNode(ret, data);
	}
	return ret;
}

void DbFsDriver::cacheRemove(const QString &path, bool post_notify)
{
	{
		auto it = m_attributeCache.lowerBound(path);
		while(it != m_attributeCache.end()) {
			if(it.key().startsWith(path)) {
				it = m_attributeCache.erase(it);
			}
			else {
				break;
			}
		}
	}
	{
		auto it = m_directoryCache.lowerBound(path);
		while(it != m_directoryCache.end()) {
			if(it.key().startsWith(path)) {
				it = m_directoryCache.erase(it);
			}
			else {
				break;
			}
		}
	}
	if(post_notify) {
		postAttributesChangedNotify(path);
	}
}

void DbFsDriver::postAttributesChangedNotify(const QString &path)
{
	Query q(connection());
	QString qs = "NOTIFY " + CHANNEL_INVALIDATE_DBFSDRIVER_CACHE + ", '" + path + "'";
	sqlDebug() << qs;
	if(!q.exec(qs)) {
		qfError() << "postAttributesChangedNotify Error:" << qs << q.lastError().text();
	}
}

void DbFsDriver::onSqlNotify(const QString &channel, QSqlDriver::NotificationSource source, const QVariant payload)
{
	qfLogFuncFrame();
	if(source != QSqlDriver::SelfSource) {
		if(channel == CHANNEL_INVALIDATE_DBFSDRIVER_CACHE) {
			QString path = payload.toString();
			cacheRemove(path, !O_POST_NOTIFY);
		}
	}
}

DbFsAttrs DbFsDriver::sqlInsertNode(const DbFsAttrs &attrs, const QByteArray &data)
{
	qfLogFuncFrame() << attrs.toString();
	int copy_of_id = attrs.id();
	Connection conn = connection();
	Query q(conn);
	QString qs = "SELECT nextval('" + tableName() + "_id_seq')";
	sqlDebug() << qs;
	bool ok = q.exec(qs);
	if(!ok) {
		qfError() << "SQLINSERTNODE Error:" << qs << '\n' << q.lastError().text();
		return DbFsAttrs();
	}
	q.next();
	int id = q.value(0).toInt();
	if(id <= 0) {
		qfError() << "SQLINSERTNODE internal error, sequence number is invalid!";
		return DbFsAttrs();
	}
	DbFsAttrs ret = attrs;
	ret.setId(id);
	ret.setInode(id);
	//ret.setSnapshot(latestSnapshotNumber());
	qs = "INSERT INTO " + tableName() + " ("
			+ COL_ID + ", "
			+ COL_INODE + ", "
			+ COL_PINODE + ", "
			+ COL_SNAPSHOT + ", "
			+ COL_MTIME + ", "
			+ COL_TYPE + ", "
			+ COL_DELETED + ", "
			+ COL_NAME + ", "
			+ COL_DATA
			+ ") "
			+ "VALUES ("
			+ QString::number(ret.id()) + ", "
			+ QString::number(ret.inode()) + ", "
			+ QString::number(ret.pinode()) + ", "
			+ QString::number(ret.snapshot()) + ", "
			+ "now(), "
			+ '\'' + ret.typeChar() + "', "
			+ (ret.isDeleted()? "true": "false") + ", "
			+ '\'' + ret.name() + "', "
			+ ":data"
			+ ")";
	ok = q.prepare(qs);
	if(!ok) {
		qfError() << "SQLINSERTNODE Error:" << qs << '\n' << q.lastError().text();
		return DbFsAttrs();
	}
	q.bindValue(":data", data);
	sqlDebug() << qs;
	ok = q.exec();
	if(!ok) {
		qfError() << "SQLINSERTNODE Error:" << qs << '\n' << q.lastError().text();
		return DbFsAttrs();
	}
	if(copy_of_id > 0) {
		/// mark clonned node as copied
		qs = "UPDATE " + tableName() + " SET "
				+ COL_COPIED_IN_SNAPSHOT + "=" + QString::number(ret.snapshot())
				+ COL_MTIME + "=now()"
				+ " WHERE id=" + QString::number(copy_of_id);
		sqlDebug() << qs;
		ok = q.exec(qs);
		if(!ok) {
			qfError() << "SQLINSERTNODE Error:" << qs << '\n' << q.lastError().text();
			return DbFsAttrs();
		}
	}
	return ret;
}

DbFsAttrs DbFsDriver::sqlDeleteNode(const DbFsAttrs &attrs)
{
	qfLogFuncFrame() << attrs.toString();
	Query q(connection());

	/// find previous snapshot version of this node
	QString qs = "SELECT id FROM " + tableName()
			+ " WHERE " + COL_INODE + "=" + QString::number(attrs.inode())
			+ " AND " + COL_SNAPSHOT + "<" + QString::number(attrs.snapshot())
			+ " ORDER BY " + COL_SNAPSHOT + " LIMIT 1";
	sqlDebug() << qs;
	bool ok = q.exec(qs);
	if(!ok) {
		qfError() << "SQLDELETENODE Error:" << qs << '\n' << q.lastError().text();
		return DbFsAttrs();
	}
	else {
		if(q.next()) {
			int id = q.value(0).toInt();
			if(id > 0) {
				qs = "UPDATE " + tableName() + " SET "
						+ COL_COPIED_IN_SNAPSHOT + "=-1, "
						+ COL_MTIME + "=now()"
						+ " WHERE id=" + QString::number(id);
				sqlDebug() << qs;
				ok = q.exec(qs);
				if(!ok) {
					qfError() << "SQLDELETENODE Error clear copy in snapshot flag:" << qs << '\n' << q.lastError().text();
					return DbFsAttrs();
				}
			}
			else {
				qfError() << "SQLDELETENODE Internal Error: previous snapshot id == 0!";
			}
		}
	}
	qs = "DELETE FROM " + tableName() + " WHERE id=" + QString::number(attrs.id());
	sqlDebug() << qs;
	ok = q.exec(qs);
	if(!ok) {
		qfError() << "SQLDELETENODE Error:" << qs << '\n' << q.lastError().text();
		return DbFsAttrs();
	}
	return attrs;
}

DbFsAttrs DbFsDriver::sqlUpdateNode(const DbFsAttrs &attrs, const QByteArray &data)
{
	qfLogFuncFrame() << attrs.toString();
	Connection conn = connection();
	Query q(conn);
	DbFsAttrs ret = attrs;
	QString qs = "UPDATE " + tableName() + " SET "
			+ COL_MTIME + "=now(), "
			+ COL_DELETED + "=" + (ret.isDeleted()? "true": "false") + ", "
			+ COL_DATA + "=:data"
			+ " WHERE id=" + QString::number(ret.id());
	bool ok = q.prepare(qs);
	if(!ok) {
		qfError() << "SQLUPDATENODE Error:" << qs << '\n' << q.lastError().text();
		return DbFsAttrs();
	}
	q.bindValue(":data", data);
	sqlDebug() << qs;
	ok = q.exec();
	if(!ok) {
		qfError() << "SQLUPDATENODE Error:" << qs << '\n' << q.lastError().text();
		return DbFsAttrs();
	}
	int n = q.numRowsAffected();
	if(n == 0) {
		qfError() << "SQLUPDATENODE Error 0 rows affected:" << qs;
		return DbFsAttrs();
	}
	return ret;
}

bool DbFsDriver::createSnapshot(const QString &comment)
{
	QString qs = "INSERT INTO " + snapshotsTableName() + " (" + COL_SS_NO + ", " + COL_SS_COMMENT + ")" +
			" select COALESCE(MAX(" + COL_SS_NO + "), -1) + 1, '" + comment + "' FROM " + snapshotsTableName();
	Connection conn = connection();
	Query q(conn);
	sqlDebug() << qs;
	bool ret = q.exec(qs);
	if(!ret) {
		qfError() << "CREATESNAPSHOT Error:" << qs << '\n' << q.lastError().text();
	}
	m_latestSnapshotNumber = -1;
	return ret;
}

qf::core::utils::Table DbFsDriver::listSnapshots()
{
	qf::core::model::SqlTableModel m;
	m.setConnectionName(connectionName());
	QString qs = "SELECT " + COL_SS_NO + ", " + COL_SS_TS + ", " + COL_SS_COMMENT + " FROM " + snapshotsTableName() + " ORDER BY " + COL_SS_NO;
	m.reload(qs);
	return m.table();
}

int DbFsDriver::latestSnapshotNumber()
{
	if(m_latestSnapshotNumber < 0)
		m_latestSnapshotNumber = readLatestSnapshotNumber();
	return m_latestSnapshotNumber;
}

bool DbFsDriver::isSnapshotReadOnly()
{
	bool read_only = snapshotNumber() < latestSnapshotNumber();
	return read_only;
}

static bool execQueryList(Connection &conn, const QStringList &qlst)
{
	bool ret = true;
	Query q(conn);
	for(QString qs : qlst) {
		sqlDebug() << qs;
		ret = q.exec(qs);
		if(!ret) {
			qfError() << "SQL Error:" << qs << '\n' << q.lastError().text();
			break;
		}
	}
	return ret;
}

bool DbFsDriver::createDbFs()
{
	qfLogFuncFrame();
	bool init_ok = false;
	Connection conn = connection();
	do {
		Query q(conn);
		init_ok = q.exec("SELECT COUNT(*) FROM " + tableName());
		if(init_ok) {
			qfWarning() << "Cannot create DBFS, table" << tableName() << "exists already!";
			return false;
		}
		init_ok = q.exec("SELECT COUNT(*) FROM " + snapshotsTableName());
		if(init_ok) {
			qfWarning() << "Cannot create DBFS, snapshots table" << snapshotsTableName() << "exists already!";
			return false;
		}
		Transaction transaction(conn);
		{
			/// create table
			qfInfo() << "Creating table:" << tableName();
			QStringList qlst;
			qlst << "CREATE TABLE " + tableName() + " " +
					"("
					"id serial NOT NULL,"
					"inode integer NOT NULL DEFAULT -1,"
					"pinode integer NOT NULL DEFAULT -1,"
					"snapshot integer NOT NULL DEFAULT -1,"
					"" + COL_MTIME + " timestamp without time zone DEFAULT now(),"
					"type character(1) NOT NULL DEFAULT 'f'::bpchar,"
					"deleted boolean NOT NULL DEFAULT false,"
					"" + COL_COPIED_IN_SNAPSHOT + " int NOT NULL DEFAULT -1,"
					"name character varying,"
					"meta character varying,"
					"" + COL_DATA + " bytea,"
					"CONSTRAINT " + tableName() + "_pkey PRIMARY KEY (id),"
					"CONSTRAINT " + tableName() + "_inode_key UNIQUE (inode, snapshot)"
					") WITH (OIDS=FALSE)";
			qlst << "COMMENT ON COLUMN " + tableName() + ".id IS 'unique number of file or directory, more versions can have same inode value but different modified time'";
			qlst << "COMMENT ON COLUMN " + tableName() + ".pinode IS 'number of parent directory inode'";
			qlst << "COMMENT ON COLUMN " + tableName() + ".snapshot IS 'Number of snapshot to which this file belongs, used for copy on write snapshots implementation.'";
			qlst << "COMMENT ON COLUMN " + tableName() + "." + COL_COPIED_IN_SNAPSHOT + " IS 'Number of snapshot to which this file was copied during copy on write snapshots implementation, used in cached file attributes validation algorithm.'";
			qlst << "CREATE INDEX " + tableName() + "_inode_idx ON " + tableName() + " (inode)";
			qlst << "CREATE INDEX " + tableName() + "_pinode_idx ON " + tableName() + " (pinode)";
			init_ok = execQueryList(conn, qlst);
			if(!init_ok) {
				qfError() << "Error creating DBFS table" << tableName();
				break;
			}
		}
		{
			/// create snapshots table
			qfInfo() << "Creating snapshots table:" << snapshotsTableName();
			QStringList qlst;
			qlst << "CREATE TABLE " + snapshotsTableName() + " " +
					"("
					"id serial NOT NULL,"
					"" + COL_SS_TS + " timestamp without time zone DEFAULT now(),"
					"" + COL_SS_COMMENT + " character varying,"
					"" + COL_SS_NO + " integer,"
					"CONSTRAINT " + snapshotsTableName() + "_pkey PRIMARY KEY (id)"
					") WITH (OIDS=FALSE)";
			qlst << "CREATE INDEX " + snapshotsTableName() + "_" + COL_SS_NO + "_idx ON " + snapshotsTableName() + " (" + COL_SS_NO + ")";
			init_ok = execQueryList(conn, qlst);
			if(!init_ok) {
				qfError() << "Error creating snapshots table" << snapshotsTableName();
				break;
			}
		}
		init_ok = createSnapshot("DBFS init");
		if(!init_ok)
			break;
		transaction.commit();
	} while(false);
	return init_ok;
}

QString DbFsDriver::attributesColumns(const QString &table_alias)
{
	QString ta = table_alias;
	if(!ta.isEmpty())
		ta += '.';
	QString ret =
			ta%COL_ID%", "%
			ta%COL_INODE%", "%
			ta%COL_PINODE%", "%
			ta%COL_SNAPSHOT%", "%
			ta%COL_MTIME%", "%
			ta%COL_TYPE%", "%
			ta%COL_DELETED%", "%
			ta%COL_COPIED_IN_SNAPSHOT%", "%
			ta%COL_NAME%", "%
			"length("%ta%COL_DATA%") AS "%COL_SIZE%", "%
			ta%COL_META;
	return ret;
}

DbFsAttrs DbFsDriver::attributesFromQuery(const Query &q)
{
	DbFsAttrs ret;
	ret.setId(q.value(COL_ID).toInt());
	ret.setInode(q.value(COL_INODE).toInt());
	ret.setPinode(q.value(COL_PINODE).toInt());
	ret.setSnapshot(q.value(COL_SNAPSHOT).toInt());
	ret.setMtime(q.value(COL_MTIME).toDateTime());
	ret.setDeleted(q.value(COL_DELETED).toBool());
	ret.setCopiedInSnapshot(q.value(COL_COPIED_IN_SNAPSHOT).toInt());
	ret.setName(q.value(COL_NAME).toString());
	DbFsAttrs::NodeType node_type = DbFsAttrs::Invalid;
	QString s = q.value(COL_TYPE).toString();
	if(!s.isEmpty()) {
		QChar c = s.at(0).toLower();
		if(c == 'd')
			node_type = DbFsAttrs::Dir;
		else if(c == 'f')
			node_type = DbFsAttrs::File;
	}
	ret.setSize(q.value(COL_SIZE).toInt());
	ret.setType(node_type);
	return ret;
}

DbFsAttrs DbFsDriver::readAttrs(const QString &path, int pinode)
{
	qfLogFuncFrame() << "path:" << path;
	static DbFsAttrs root_attrs;
	if(root_attrs.isNull()) {
		root_attrs.setType(DbFsAttrs::Dir);
		root_attrs.setId(0);
		root_attrs.setInode(0);
		root_attrs.setPinode(0);
	}

	QStringList pathlst = splitPath(path);

	DbFsAttrs ret;
	if(pathlst.isEmpty()) {
		ret = root_attrs;
	}
	else {
		//QString top_tbl = "t0";// + QString::number(pathlst.count());
		QString cols = attributesColumns();
		QString single_select =
				"SELECT %3 FROM " + tableName() + " WHERE NOT deleted AND (inode,snapshot) = (\n" +
				"  SELECT inode, MAX(snapshot) FROM " + tableName() + " WHERE name='%1' AND pinode=(%2) AND snapshot<=" + QString::number(snapshotNumber()) + " GROUP BY inode \n"
																																							  ")\n";
		QString qs = QString::number(pinode);
		for(int i=0; i<pathlst.count(); i++) {
			QString p = pathlst.value(i);
			qs = single_select.arg(p).arg(qs);
			if(i == pathlst.count() - 1)
				qs = qs.arg(cols);
			else
				qs = qs.arg(QStringLiteral("inode"));
		}
		//qs = "SELECT " + cols + " FROM " + tableName() + " AS t0 WHERE (inode,snapshot) = (\n" + qs + ")";
		Connection conn = connection();
		Query q(conn);
		sqlDebug() << qs;
		if(q.exec(qs)) {
			if(q.next()) {
				ret = attributesFromQuery(q);
			}
			else {
				//qfWarning() << "QFDbFs::pathToId() ERROR - table:" << tableName() << "parent id:" << pinode << "path:" << path.join("/") << "not found.";
			}
		}
		else {
			qfInfo() << qs;
			qfError() << "SQL ERROR:" << qs << '\n' << q.lastError().text();
			throw qf::core::Exception("bye");
		}
	}
	qfDebug() << "\t read attrs for path:" << path << ret.toString();
	return ret;
}

QList<DbFsAttrs> DbFsDriver::readChildAttrs(int parent_inode)
{
	qfLogFuncFrame() << "parent_inode:" << parent_inode;
	QList<DbFsAttrs> ret;
	Connection conn = connection();
	Query q(conn);
	QString cols = attributesColumns("t2");
	QString inner_qs = "SELECT inode, MAX(snapshot) AS ms FROM " + tableName()
			+ " WHERE " + COL_PINODE + '=' + QString::number(parent_inode)
			+ "  AND snapshot<=" + QString::number(snapshotNumber())
			+ " GROUP BY inode";
	QString qs = "SELECT " + cols + " FROM ( " + inner_qs + " ) AS t1" +
			" JOIN " + tableName() + " AS t2 ON t1.inode=t2.inode AND t1.ms=t2.snapshot AND NOT t2.deleted";
	sqlDebug() << qs;
	bool ok = q.exec(qs);
	if(ok) {
		while (q.next()) {
			DbFsAttrs att = attributesFromQuery(q);
			qfDebug() << "\t adding child:" << att.toString();
			ret << att;
		}
	}
	else {
		qfError() << "SQL ERROR:" << qs << '\n' << q.lastError().text();
	}
	return ret;
}

int DbFsDriver::readLatestSnapshotNumber()
{
	int ret = -1;
	Connection conn = connection();
	Query q(conn);
	QString qs = "SELECT MAX(" + COL_SS_NO + ") FROM " + snapshotsTableName();
	sqlDebug() << qs;
	bool ok = q.exec(qs);
	if(!ok) {
		qfError() << "SQL Error:" << qs << '\n' << q.lastError().text();
	}
	else {
		if(q.next())
			ret = q.value(0).toInt();
	}
	return ret;
}

QByteArray DbFsDriver::get(const QString &path, bool *pok)
{
	qfLogFuncFrame() << path;
	QByteArray ret;
	QString spath = cleanPath(path);
	bool ok = false;
	do {
		DbFsAttrs attrs = attributes(spath);
		if(attrs.isNull()) {
			qfWarning() << "Cannot get attributes for:" << spath;
			break;
		}
		int id = attrs.id();
		QString cols = attributesColumns() + ", " + COL_DATA;
		QString qs = "SELECT " + cols + " FROM " + tableName() + " WHERE id=" + QString::number(id);
		Connection conn = connection();
		Query q(conn);
		sqlDebug() << qs;
		if(!q.exec(qs)) {
			qfError() << "Error get file:" << qs << '\n' << q.lastError().text();
			break;
		}
		if(!q.next()) {
			qfError() << "Error get file: empty result set!";
			break;
		}
		DbFsAttrs attrs2 = attributesFromQuery(q);
		if(attrs2.mtime() > attrs.mtime()) {
			/// cached attributes are older than loded ones, cache record may not be valid
			int sn2 = attrs2.copiedInSnapshot();
			if(sn2 > attrs2.snapshot() && sn2 <= snapshotNumber()) {
				/// there is copy on write in more recent snapshot
				/// loaded data is overwritten in newer snapshot
				/// clear cache and load data again
				qfDebug() << "Cached data invalid, reloading cache and data again.";
				cacheRemove(spath, !O_POST_NOTIFY);
				continue;
			}
			else {
				/// there is NOT copy on write in more recent snapshot
				/// data is valid, only cach refresh is needed
				qfDebug() << "Cached data invalid and updated from data query.";
				m_attributeCache[spath] = attrs2;
			}
		}
		ret = q.value(COL_DATA).toByteArray();
		ok = true;
	} while(false);
	if(pok)
		*pok = ok;
	qfDebug() << "\t returned" << ret.length() << "bytes od data:" << ((ret.size() < 100)? ret : ret.mid(100));
	return ret;
}

bool DbFsDriver::put(const QString &path, const QByteArray &data)
{
	qfLogFuncFrame() << path << ((data.size() < 100)? data : data.mid(100));
	if(!checkWritePermissions())
		return false;

	QString spath = cleanPath(path);

	bool ok = false;
	do {
		DbFsAttrs att = attributes(spath);
		if(att.isNull()) {
			qfWarning() << "PUT to invalid path:" << spath;
			break;
		}
		if(att.type() == DbFsAttrs::Dir) {
			qfWarning() << "PUT to directory:" << spath;
			break;
		}
		cacheRemove(spath, O_POST_NOTIFY);
		TableLocker locker(connection(), tableName(), O_LOCK_EXCLUSIVE);
		att = touch(att, !O_CREATE, !O_DELETE, data);
		if(att.isNull()) {
			qfWarning() << "ERROR detach node on path:" << spath;
			break;
		}
		locker.commit();
		ok = true;
	} while(false);
	return ok;
}

bool DbFsDriver::mkfile(const QString &path, const QByteArray &data)
{
	qfLogFuncFrame() << path;
	bool ok = mknod(path, DbFsAttrs::File, data);
	return ok;
}

bool DbFsDriver::mkdir(const QString &path)
{
	qfLogFuncFrame() << path;
	bool ok = mknod(path, DbFsAttrs::Dir, QByteArray());
	return ok;
}

bool DbFsDriver::mknod(const QString &path, DbFsAttrs::NodeType node_type, const QByteArray &data)
{
	qfLogFuncFrame() << path;
	if(!checkWritePermissions())
		return false;

	TableLocker locker(connection(), tableName(), O_LOCK_EXCLUSIVE);
	QString spath = cleanPath(path);
	QPair<QString, QString> pf = splitPathFile(spath);

	bool ok = false;
	do {
		if(pf.second.isEmpty()) {
			qfWarning() << "Cannot create node with empty name!";
			break;
		}
		cacheRemove(pf.first, !O_POST_NOTIFY);
		DbFsAttrs att = attributes(spath);
		if(!att.isNull()) {
			qfWarning() << "MKNOD path exists:" << spath;
			break;
		}
		DbFsAttrs patt = attributes(pf.first);
		if(patt.isNull()) {
			qfWarning() << "MKNOD parent path not exists:" << pf.first;
			break;
		}
		if(patt.type() != DbFsAttrs::Dir) {
			qfWarning() << "MKNOD parent path is not directory:" << pf.first;
			break;
		}
		att.setName(pf.second);
		att.setType(node_type);
		att.setPinode(patt.inode());
		att = touch(att, O_CREATE, !O_DELETE, data);
		if(att.isNull()) {
			qfWarning() << "MKNOD ERROR create node type:" + att.typeName() + " on path:" << spath;
			break;
		}
		locker.commit();
		cacheRemove(pf.first, O_POST_NOTIFY);
		ok = true;
	} while(false);
	return ok;
}

bool DbFsDriver::rmnod(const QString &path)
{
	qfLogFuncFrame() << path;
	if(!checkWritePermissions())
		return false;

	TableLocker locker(connection(), tableName(), O_LOCK_EXCLUSIVE);
	QString spath = cleanPath(path);
	QPair<QString, QString> pf = splitPathFile(spath);

	bool ok = false;
	do {
		cacheRemove(spath, !O_POST_NOTIFY);
		DbFsAttrs att = attributes(spath);
		if(att.isNull()) {
			qfWarning() << "RMNOD path not exists:" << spath;
			break;
		}
		if(att.type() == DbFsAttrs::Dir) {
			if(!childAttributes(spath).isEmpty()) {
				qfWarning() << "RMNOD cannot remove directory, because it has children:" << spath;
				break;
			}
		}
		att = touch(att, !O_CREATE, O_DELETE, QByteArray());
		if(att.isNull()) {
			qfWarning() << "RMNOD ERROR rmove node type:" + att.typeName() + " on path:" << spath;
			break;
		}
		locker.commit();
		cacheRemove(pf.first, O_POST_NOTIFY);
		ok = true;
	} while(false);
	return ok;
}

QPair<QString, QString> DbFsDriver::splitPathFile(const QString &path)
{
	QPair<QString, QString> ret;
	int ix = path.lastIndexOf('/');
	if(ix < 0) {
		ret.second = path;
	}
	else {
		ret.first = path.mid(0, ix);
		ret.second = path.mid(ix + 1);
	}
	return ret;
}

QStringList DbFsDriver::splitPath(const QString &path)
{
	QStringList ret = path.split('/', QString::SkipEmptyParts);
	return ret;
}

QString DbFsDriver::joinPath(const QString &p1, const QString &p2)
{
	QString ret = p1;
	if(!p1.isEmpty())
		ret += '/';
	ret += p2;
	return ret;
}

QString DbFsDriver::cleanPath(const QString &path)
{
	QString ret = QDir::cleanPath(path);
	while(ret.startsWith('/'))
		ret = ret.mid(1);
	while(ret.endsWith('/'))
		ret = ret.mid(0, ret.length() - 1);
	//if(ret.isEmpty() && !path.isEmpty() && path[0] == '/')
	//	ret = "/";
	return ret;
}

