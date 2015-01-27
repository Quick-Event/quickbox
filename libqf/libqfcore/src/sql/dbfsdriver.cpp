#include "dbfsdriver.h"
#include "connection.h"
#include "query.h"

#include <QSqlDatabase>
#include <QSqlError>
#include <QDir>
#include <QStringBuilder>

using namespace qf::core::sql;

static const QString COL_ID("id");
static const QString COL_INODE("inode");
static const QString COL_PINODE("pinode");
static const QString COL_TYPE("type");
static const QString COL_NAME("name");
static const QString COL_META("meta");
static const QString COL_SNAPSHOT("snapshot");
static const QString COL_DELETED("deleted");
static const QString COL_VALUE("value");
static const QString COL_SIZE("size");

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
	qfLogFuncFrame() << path;
	QString spath = cleanPath(path);
	DbFsAttrs ret = m_attributeCache.value(spath);
	if(ret.isNull()) {
		if(path.isEmpty()) {
			/// create fake root node
			ret.setType(DbFsAttrs::Dir);
			ret.setId(0);
			ret.setInode(0);
			ret.setPinode(0);
		}
		else {
			ret = readAttrs(path);
		}
		/*
		ret = readAttrs(path);
		if(ret.isNull()) {
			if(path == "/") {
				if(initDbFs()) {
					ret = readAttrs(path);
					if(ret.isNull()) {
						qfError() << "Cannot get DBFS root attributes, this is internal error!";
					}
				}
			}
			else {
				qfWarning() << "Cannot get DBFS attributes for path:" << path;
			}
		}
		*/
		if(!ret.isNull()) {
			m_attributeCache[path] = ret;
		}
	}
	qfDebug() << ret.toString();
	return ret;
}

QList<DbFsAttrs> DbFsDriver::readDir(const QString &parent_path)
{
	qfLogFuncFrame() << parent_path;
	QList<DbFsAttrs> ret;
	QString clean_ppath = cleanPath(parent_path);
	qfDebug() << "\t cleaned path:" << clean_ppath;
	DbFsAttrs parent_attrs = attributes(clean_ppath);
	if(!parent_attrs.isNull() && parent_attrs.type() == DbFsAttrs::Dir) {
		int parent_inode = parent_attrs.inode();
		ret = readDir(parent_inode);
		for(auto attrs : ret) {
			QString path = clean_ppath;
			if(!path.isEmpty())
				path += '/';
			path += attrs.name();
			//qfDebug() << clean_ppath + "name:" << attrs.name() << "->" << path;
			m_attributeCache[path] = attrs;
		}
	}
	else {
		qfWarning() << "Node on path:" << parent_path << "is not dir:" << parent_attrs.toString();
	}
	return ret;
}

Connection DbFsDriver::connection()
{
	QSqlDatabase db = QSqlDatabase::database(connectionName(), false);
	QF_ASSERT_EX(db.isOpen(), tr("Connection '%1' is not open!").arg(connectionName()));
	return Connection(db);
}

bool DbFsDriver::createSnapshot(const QString &comment)
{
	QString qs = "INSERT INTO " + snapshotsTableName() + " (ssNo, comment)" +
			" select COALESCE(MAX(ssNo), -1) + 1, '" + comment + "' FROM " + snapshotsTableName();
	Connection conn = connection();
	Query q(conn);
	bool ret = q.exec(qs);
	if(!ret) {
		qfError() << "Error init dbfs:" << q.lastError().text();
	}
	return ret;
}

static bool execQueryList(Connection &conn, const QStringList &qlst)
{
	bool ret = true;
	Query q(conn);
	for(QString qs : qlst) {
		ret = q.exec(qs);
		if(!ret) {
			qfError() << "SQL Error:" << q.lastError().text();
			break;
		}
	}
	return ret;
}

bool DbFsDriver::initDbFs()
{
	bool init_ok = false;
	Connection conn = connection();
	conn.transaction();
	do {
		Query q(conn);
		init_ok = q.exec("SELECT COUNT(*) FROM " + tableName());
		if(!init_ok) {
			/// create table
			qfInfo() << "Creating table:" << tableName();
			QStringList qlst;
			qlst << "CREATE TABLE " + tableName() + " " +
					"("
					"id serial NOT NULL,"
					"inode integer NOT NULL DEFAULT 0,"
					"pinode integer NOT NULL DEFAULT 0,"
					"snapshot integer NOT NULL DEFAULT 0,"
					"type character(1) NOT NULL DEFAULT 'f'::bpchar,"
					"deleted boolean NOT NULL DEFAULT false,"
					"name character varying,"
					"meta character varying,"
					"value bytea,"
					"CONSTRAINT pkey PRIMARY KEY (id),"
					"CONSTRAINT inode_key UNIQUE (inode, snapshot)"
					")"
					"WITH ("
					"OIDS=FALSE"
					")";
			qlst << "COMMENT ON COLUMN " + tableName() + ".id IS 'unique number of file or directory, more versions can have same inode value but different modified time'";
			qlst << "COMMENT ON COLUMN " + tableName() + ".pinode IS 'number of parent directory inode'";
			qlst << "COMMENT ON COLUMN " + tableName() + ".snapshot IS 'Number of snapshot to which this file belongs, used for copy on write snapshots implementation.'";
			qlst << "CREATE INDEX " + tableName() + "_inode_idx ON " + snapshotsTableName() + " (inode)";
			qlst << "CREATE INDEX " + tableName() + "_pinode_idx ON " + snapshotsTableName() + " (pinode)";
			init_ok = execQueryList(conn, qlst);
			if(!init_ok) {
				qfError() << "Error creating DBFS table" << tableName();
				break;
			}
			init_ok = q.exec("SELECT COUNT(*) FROM " + tableName());
			if(!init_ok)
				break;
		}
		if(q.next() && q.value(0).toInt() == 0) {
			init_ok = q.exec("SELECT COUNT(*) FROM " + snapshotsTableName());
			if(!init_ok) {
				/// create snapshots table
				qfInfo() << "Creating snapshots table:" << snapshotsTableName();
				QStringList qlst;
				qlst << "CREATE TABLE " + snapshotsTableName() + " " +
						"("
						"id serial NOT NULL,"
						"ts timestamp without time zone DEFAULT now(),"
						"comment character varying,"
						"ssNo integer,"
						"CONSTRAINT pkey PRIMARY KEY (id)"
						")"
						"WITH ("
						"OIDS=FALSE"
						")";
				qlst << "CREATE INDEX " + snapshotsTableName() + "_ssNo_idx ON " + snapshotsTableName() + " (ssNo)";
				init_ok = execQueryList(conn, qlst);
				if(!init_ok) {
					qfError() << "Error creating snapshots table" << snapshotsTableName();
					break;
				}
				init_ok = q.exec("SELECT COUNT(*) FROM " + snapshotsTableName());
				if(!init_ok)
					break;
			}
			if(q.next() && q.value(0).toInt() == 0) {
				init_ok = createSnapshot("DBFS init");
				if(!init_ok)
					break;
			}
			else {
				qfError() << "Cannot init filesystem snapshot table" << tableName() << "because it is not empty.";
				init_ok = false;
				break;
			}
		}
		else {
			qfError() << "Cannot init DBFS table" << tableName() << "because it is not empty.";
			init_ok = false;
			break;
		}
		init_ok = true;
	} while(false);
	if(init_ok)
		conn.commit();
	else
		conn.rollback();
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
			ta%COL_TYPE%", "%
			ta%COL_DELETED%", "%
			ta%COL_NAME%", "%
			"length("%ta%COL_VALUE%") AS "%COL_SIZE%", "%
			ta%COL_META;
	return ret;
}

DbFsAttrs DbFsDriver::attributesFromQuery(const Query &q)
{
	DbFsAttrs ret;
	ret.setId(q.value(COL_ID).toInt());
	ret.setInode(q.value(COL_INODE).toInt());
	ret.setPinode(q.value(COL_PINODE).toInt());
	ret.setDeleted(q.value(COL_DELETED).toBool());
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
	static DbFsAttrs root_attrs(DbFsAttrs::Dir);

	QStringList pathlst = path.split('/', QString::SkipEmptyParts);

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
		qfDebug() << qs;
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
			qfError() << "SQL ERROR:" << q.lastError().text();
			throw qf::core::Exception("bye");
		}
	}
	qfDebug() << ret.toString();
	return ret;
}

QList<DbFsAttrs> DbFsDriver::readDir(int parent_inode)
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
	QString qs = "SELECT " + cols + " FROM ( " + inner_qs + " ) AS t1"
															" JOIN dbfs AS t2 ON t1.inode=t2.inode AND t1.ms=t2.snapshot";
	qfDebug() << qs;
	bool ok = q.exec(qs);
	if(ok) {
		while (q.next()) {
			DbFsAttrs att = attributesFromQuery(q);
			qfDebug() << "\t adding child:" << att.toString();
			ret << att;
		}
	}
	else {
		qfError() << "SQL ERROR:" << q.lastError().text();
	}
	return ret;
}

QByteArray DbFsDriver::get(const QString &path, bool *pok)
{
	QByteArray ret;
	QString spath = cleanPath(path);
	bool ok = false;
	do {
		DbFsAttrs attrs = attributes(spath);
		if(attrs.isNull())
			break;
		int id = attrs.id();
		QString qs = "SELECT " + COL_VALUE + " FROM " + tableName() + " WHERE id=" + QString::number(id);
		Connection conn = connection();
		Query q(conn);
		if(!q.exec(qs)) {
			qfError() << "Error get file:" << q.lastError().text();
			break;
		}
		if(!q.next()) {
			qfError() << "Error get file: empty result set!";
			break;
		}
		ret = q.value(0).toByteArray();
		ok = true;
	} while(false);
	if(pok)
		*pok = ok;
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

