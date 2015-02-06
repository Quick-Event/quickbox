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

//static const QString COL_ID("id");
static const QString COL_INODE("inode");
static const QString COL_PINODE("pinode");
static const QString COL_TYPE("type");
static const QString COL_NAME("name");
static const QString COL_META("meta");
static const QString COL_MTIME("mtime");
static const QString COL_DATA("data");
static const QString COL_SIZE("size");

static const QString O_LOCK_EXCLUSIVE = QStringLiteral("EXCLUSIVE");
//static const bool O_CREATE = true;
//static const bool O_DELETE = true;
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
		DbFsAttrs a = readAttrs(spath, 0);
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
	qfDebug() << "\t cleaned parent path:" << clean_ppath;
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

bool DbFsDriver::checkWritePermissions()
{
	bool ok = true;
	return ok;
}

static QByteArray truncateArray(const QByteArray &ba, int new_size)
{
	QByteArray ret = ba;
	if(new_size >= 0) {
		int orig_size = ret.size();
		ret.resize(new_size);
		for(int i=orig_size; i<new_size; i++)
			ret[i] = 0;
	}
	return ret;
}

DbFsAttrs DbFsDriver::put_helper(const QString &spath, const DbFsAttrs &new_attrs, const QByteArray &data, int options, int new_size)
{
	qfLogFuncFrame() << spath;
	DbFsAttrs ret;
	if(!checkWritePermissions())
		return ret;

	do {
		TableLocker locker(connection(), tableName(), O_LOCK_EXCLUSIVE);
		cacheRemove(spath, !O_POST_NOTIFY);
		QString invalid_cache_path = spath;

		DbFsAttrs att = attributes(spath);
		if(att.isNull()) {
			if(options & PN_CREATE) {
				if(new_attrs.type() == DbFsAttrs::Invalid) {
					qfWarning() << "cannot create invalid node type";
					break;
				}
				QPair<QString, QString> pf = splitPathFile(spath);
				DbFsAttrs patt = attributes(pf.first);
				if(patt.isNull()) {
					qfWarning() << "cannot create node:" << spath << " parent dir not exits";
					break;
				}
				if(patt.type() != DbFsAttrs::Dir) {
					qfWarning() << "cannot create node:" << spath << " parent path is not dir";
					break;
				}
				QByteArray ba = data;
				if(options & PN_TRUNCATE) {
					ba = truncateArray(ba, new_size);
				}
				att.setName(pf.second);
				att.setType(new_attrs.type());
				att.setPinode(patt.inode());
				ret = sqlInsertNode(att, ba);
				invalid_cache_path = pf.first;
			}
			else {
				qfWarning() << "PUT to not existing path:" << spath;
				break;
			}
		}
		else {
			if(!(options & PN_OVERRIDE)) {
				qfError() << "Internal Error - cannot override node:" << spath;
				break;
			}
			if(options & PN_DELETE) {
				if(att.type() == DbFsAttrs::Dir && !childAttributes(spath).isEmpty()) {
					qfWarning() << "cannot delete not empty path:" << spath;
					break;
				}
				if(sqlDeleteNode(att.inode()))
					ret = att;
			}
			else if(options & PN_RENAME) {
				if(sqlRenameNode(att.inode(), att.name())) {
					ret = att;
					QPair<QString, QString> pf = splitPathFile(spath);
					invalid_cache_path = pf.first;
				}
			}
			else {
				if(att.type() != DbFsAttrs::File) {
					qfWarning() << "Data can be put to FILE node only:" << spath;
					break;
				}
				QByteArray ba = data;
				if(options & PN_TRUNCATE) {
					DbFsAttrs a = sqlSelectNode(att.inode(), &ba);
					if(a.isNull()) {
						qfWarning() << "Data canot be loaded to be truncated:" << spath;
						break;
					}
					ba = truncateArray(ba, new_size);
				}
				if(sqlUpdateNode(att.inode(), ba))
					ret = att;
			}
		}
		locker.commit();
		cacheRemove(invalid_cache_path, O_POST_NOTIFY);
	} while(false);

	return ret;
}
/*
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
*/
void DbFsDriver::cacheRemove(const QString &path, bool post_notify)
{
	{
		auto it = m_attributeCache.lowerBound(path);
		while(it != m_attributeCache.end()) {
			QString s = it.key();
			if(s.startsWith(path)) {
				if(s.length() == path.length() || s[path.length()] == '/')
					it = m_attributeCache.erase(it);
				else
					++it;
			}
			else {
				break;
			}
		}
	}
	{
		auto it = m_directoryCache.lowerBound(path);
		while(it != m_directoryCache.end()) {
			QString s = it.key();
			if(s.startsWith(path)) {
				if(s.length() == path.length() || s[path.length()] == '/')
					it = m_directoryCache.erase(it);
				else
					++it;
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

DbFsAttrs DbFsDriver::sqlSelectNode(int inode, QByteArray *pdata)
{
	DbFsAttrs ret;
	QString cols = attributesColumns();
	if(pdata)
		cols + ", " + COL_DATA;
	QString qs = "SELECT " + cols + " FROM " + tableName() + " WHERE inode=" + QString::number(inode);
	Connection conn = connection();
	Query q(conn);
	//sqlDebug() << qs;
	do {
		if(!q.exec(qs)) {
			qfError() << "Error get file:" << qs << '\n' << q.lastError().text();
			break;
		}
		if(!q.next()) {
			qfError() << "Error get file: empty result set!";
			break;
		}
		if(pdata) {
			QByteArray ba = q.value(COL_DATA).toByteArray();
			*pdata = ba;
		}
		ret = attributesFromQuery(q);
	} while(false);
	return ret;
}

DbFsAttrs DbFsDriver::sqlInsertNode(const DbFsAttrs &attrs, const QByteArray &data)
{
	qfLogFuncFrame() << attrs.toString();
	Connection conn = connection();
	Query q(conn);
	/*
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
	*/
	DbFsAttrs ret = attrs;
	//ret.setId(id);
	//ret.setSnapshot(latestSnapshotNumber());
	QString qs = "INSERT INTO " + tableName() + " ("
			//+ COL_ID + ", "
			//+ COL_INODE + ", "
			+ COL_PINODE + ", "
			+ COL_MTIME + ", "
			+ COL_TYPE + ", "
			+ COL_NAME + ", "
			+ COL_DATA
			+ ") "
			+ "VALUES ("
			//+ QString::number(ret.id()) + ", "
			//+ QString::number(ret.inode()) + ", "
			+ QString::number(ret.pinode()) + ", "
			+ "now(), "
			+ '\'' + ret.typeChar() + "', "
			+ '\'' + ret.name() + "', "
			+ ":data"
			+ ")";
	bool ok = q.prepare(qs);
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
	int inode = q.lastInsertId().toInt();
	if(inode <= 0) {
		qfError() << "SQLINSERTNODE lastInsertId Error:" << qs << '\n' << q.lastError().text();
		return DbFsAttrs();
	}
	ret.setInode(inode);
	return ret;
}

bool DbFsDriver::sqlDeleteNode(int inode)
{
	qfLogFuncFrame() << inode;
	Query q(connection());

	QString qs = "DELETE FROM " + tableName() + " WHERE inode=" + QString::number(inode);
	sqlDebug() << qs;
	bool ok = q.exec(qs);
	if(!ok) {
		qfError() << "SQLDELETENODE Error:" << qs << '\n' << q.lastError().text();
		return false;
	}
	return true;
}

bool DbFsDriver::sqlUpdateNode(int inode, const QByteArray &data)
{
	qfLogFuncFrame() << inode;
	Connection conn = connection();
	Query q(conn);
	QString qs = "UPDATE " + tableName() + " SET "
			+ COL_MTIME + "=now(), "
			+ COL_DATA + "=:data"
			+ " WHERE " + COL_INODE + "=:inode";
	bool ok = q.prepare(qs);
	if(!ok) {
		qfError() << "SQLUPDATENODE Error:" << qs << '\n' << q.lastError().text();
		return false;
	}
	q.bindValue(":data", data);
	q.bindValue(":inode", inode);
	sqlDebug() << qs;
	ok = q.exec();
	if(!ok) {
		qfError() << "SQLUPDATENODE Error:" << qs << '\n' << q.lastError().text();
		return false;
	}
	int n = q.numRowsAffected();
	if(n == 0) {
		qfError() << "SQLUPDATENODE Error 0 rows affected:" << qs;
		return false;
	}
	return true;
}

bool DbFsDriver::sqlRenameNode(int inode, const QString &new_name)
{
	qfLogFuncFrame() << inode << new_name;
	Connection conn = connection();
	Query q(conn);
	QString qs = "UPDATE " + tableName() + " SET "
			+ COL_MTIME + "=now(), "
			+ COL_NAME + "=:name"
			+ " WHERE " + COL_INODE + "=:inode";
	bool ok = q.prepare(qs);
	if(!ok) {
		qfError() << "SQLUPDATENODE Error:" << qs << '\n' << q.lastError().text();
		return false;
	}
	q.bindValue(":name", new_name);
	q.bindValue(":inode", inode);
	sqlDebug() << qs;
	ok = q.exec();
	if(!ok) {
		qfError() << "SQLUPDATENODE Error:" << qs << '\n' << q.lastError().text();
		return false;
	}
	int n = q.numRowsAffected();
	if(n == 0) {
		qfError() << "SQLUPDATENODE Error 0 rows affected:" << qs;
		return false;
	}
	return true;
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
		Transaction transaction(conn);
		{
			/// create table
			qfInfo() << "Creating table:" << tableName();
			QStringList qlst;
			qlst << "CREATE TABLE " + tableName() + " " +
					"("
					+ COL_INODE + " serial NOT NULL, "
					+ COL_PINODE + " integer NOT NULL DEFAULT -1, "
					+ COL_MTIME + " timestamp without time zone DEFAULT now(), "
					+ COL_TYPE + " character(1) NOT NULL DEFAULT 'f'::bpchar, "
					+ COL_NAME + " character varying, "
					+ COL_META + " character varying, "
					+ COL_DATA + " bytea, "
					+ "CONSTRAINT " + tableName() + "_pkey PRIMARY KEY (" + COL_INODE + ")"
					+ ") WITH (OIDS=FALSE)";
			qlst << "COMMENT ON COLUMN " + tableName() + ".pinode IS 'number of parent directory inode'";
			qlst << "CREATE INDEX " + tableName() + "_pinode_idx ON " + tableName() + " (pinode)";
			init_ok = execQueryList(conn, qlst);
			if(!init_ok) {
				qfError() << "Error creating DBFS table" << tableName();
				break;
			}
		}
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
			ta%COL_INODE%", "%
			ta%COL_PINODE%", "%
			ta%COL_MTIME%", "%
			ta%COL_TYPE%", "%
			ta%COL_NAME%", "%
			"length("%ta%COL_DATA%") AS "%COL_SIZE%", "%
			ta%COL_META;
	return ret;
}

DbFsAttrs DbFsDriver::attributesFromQuery(const Query &q)
{
	DbFsAttrs ret;
	ret.setInode(q.value(COL_INODE).toInt());
	ret.setPinode(q.value(COL_PINODE).toInt());
	ret.setMtime(q.value(COL_MTIME).toDateTime());
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

DbFsAttrs DbFsDriver::readAttrs(const QString &spath, int pinode)
{
	qfLogFuncFrame() << "path:" << spath;
	static DbFsAttrs root_attrs;
	if(root_attrs.isNull()) {
		root_attrs.setType(DbFsAttrs::Dir);
		root_attrs.setInode(0);
		root_attrs.setPinode(0);
	}

	QStringList pathlst = splitPath(spath);

	DbFsAttrs ret;
	if(pathlst.isEmpty()) {
		ret = root_attrs;
	}
	else {
		QString cols = attributesColumns();
		QString single_select =
				"SELECT %3 FROM " + tableName() + " WHERE inode = (\n" +
				"  SELECT inode FROM " + tableName() + " WHERE name='%1' AND pinode=%2 \n" +
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
			qfError() << "SQL ERROR:" << qs << '\n' << q.lastError().text();
			//throw qf::core::Exception("bye");
		}
	}
	qfDebug() << "\t read attrs for path:" << spath << ret.toString();
	return ret;
}

QList<DbFsAttrs> DbFsDriver::readChildAttrs(int parent_inode)
{
	qfLogFuncFrame() << "parent_inode:" << parent_inode;
	QList<DbFsAttrs> ret;
	Connection conn = connection();
	Query q(conn);
	QString cols = attributesColumns();
	QString qs = "SELECT " + cols + " FROM " + tableName()
			+ " WHERE " + COL_PINODE + '=' + QString::number(parent_inode);
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
		int inode = attrs.inode();
		QString cols = attributesColumns() + ", " + COL_DATA;
		QString qs = "SELECT " + cols + " FROM " + tableName() + " WHERE inode=" + QString::number(inode);
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
			/// cached attributes are older than loded ones, refresh cache record
			qfDebug() << "Cached data invalid and updated from data query.";
			m_attributeCache[spath] = attrs2;
		}
		ret = q.value(COL_DATA).toByteArray();
		ok = true;
	} while(false);
	if(pok)
		*pok = ok;
	qfDebug() << "\t returned" << ret.length() << "bytes od data:" << ((ret.size() < 100)? ret : ret.mid(100));
	return ret;
}

bool DbFsDriver::put(const QString &path, const QByteArray &data, bool create_if_not_exist)
{
	qfLogFuncFrame() << path << ((data.size() < 100)? data : data.mid(100));

	QString spath = cleanPath(path);
	int opts = 0;
	opts |= PN_OVERRIDE;
	if(create_if_not_exist) {
		opts |= PN_CREATE;
	}
	DbFsAttrs att = put_helper(spath, DbFsAttrs::File, data, opts, -1);
	return !att.isNull();
}

bool DbFsDriver::truncate(const QString &path, int new_size)
{
	qfLogFuncFrame() << path;

	QString spath = cleanPath(path);
	int opts = 0;
	opts |= PN_OVERRIDE;
	opts |= PN_TRUNCATE;
	//opts |= PN_CREATE;
	DbFsAttrs att = put_helper(spath, DbFsAttrs::File, QByteArray(), opts, new_size);
	return !att.isNull();
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

	QString spath = cleanPath(path);
	int opts = 0;
	//opts |= PN_OVERRIDE;
	opts |= PN_CREATE;
	DbFsAttrs att = put_helper(spath, node_type, data, opts, -1);
	return !att.isNull();
}

bool DbFsDriver::rmnod(const QString &path)
{
	QString spath = cleanPath(path);
	int opts = 0;
	opts |= PN_DELETE;
	opts |= PN_OVERRIDE;
	DbFsAttrs att = put_helper(spath, DbFsAttrs::Invalid, QByteArray(), opts, -1);
	return !att.isNull();
}

bool DbFsDriver::mv(const QString &old_path, const QString &new_path)
{
	qfLogFuncFrame() << old_path << "->" << new_path;

	QString sopath = cleanPath(old_path);
	QString snpath = cleanPath(new_path);
	QPair<QString, QString> opf = splitPathFile(sopath);
	QPair<QString, QString> npf = splitPathFile(snpath);
	if(opf.first != npf.first) {
		qfError() << old_path << "->" << new_path << "move across directories is not supported yet";
		return false;
	}
	int opts = 0;
	opts |= PN_OVERRIDE;
	opts |= PN_RENAME;
	DbFsAttrs att;
	att.setName(npf.second);
	att = put_helper(sopath, att, QByteArray(), opts, -1);
	return !att.isNull();
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

