#include "dbfsdriver.h"
#include "connection.h"
#include "query.h"

#include <QSqlDatabase>
#include <QSqlError>
#include <QDir>

using namespace qf::core::sql;

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
	DbFsAttrs ret = m_attributeCache.value(path);
	if(ret.isNull()) {
		ret = loadAttributesForPath(path);
		if(ret.isNull()) {
			if(path == "/") {
				if(initDbFs()) {
					ret = loadAttributesForPath(path);
					if(ret.isNull()) {
						qfError() << "Cannot get DBFS root attributes, this is internal error!";
					}
				}
			}
			else {
				qfWarning() << "Cannot get DBFS attributes for path:" << path;
			}
		}
		if(!ret.isNull()) {
			m_attributeCache[path] = ret;
		}
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
			" select COALESCE(MAX(ssNo), -1) + 1, 'dbfs init' FROM " + snapshotsTableName();
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
				if(init_ok) {
					QStringList qlst;
					qlst << "INSERT INTO " + tableName()
							+ " (inode, pinode, snapshot, type, deleted) VALUES"
							+ " (1, 0, 'd', false)";
					//qlst << "UPDATE " + tableName() + " SET inode=id";
					init_ok = execQueryList(conn, qlst);
					if(!init_ok)
						break;
				}
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

DbFsAttrs DbFsDriver::loadAttributesForPath(const QString &path)
{
	QString paths = QDir::cleanPath(path);
	QStringList plst = paths.split('/');
}
// http://www.postgresql.org/docs/9.3/static/queries-with.html
int DbFsDriver::pathToId(int pinode, const QStringList& path, DbFsAttrs *p_dbfs_attrs)
{
	qfLogFuncFrame() << "pinode:" << pinode << "path:" << path.join(QChar('/'));
	if(path.isEmpty()) {
		if(p_dbfs_attrs)
			*p_dbfs_attrs = DbFsAttrs();
		return pinode;
	}
	QString cache_key = QString::number(pinode) % ':' % tableName() % '/' % path.join("/");
	DbFsAttrs fi = f_pathToIdCache.value(cache_key);
	if(fi.dbId() == 0) {
		QString qs;
		QString top_tbl = "t"IARG(path.count());
		QString cols = top_tbl%'.'%COL_INODE%" AS id, "
			%top_tbl%'.'%COL_PINODE%" AS pid, "
			%top_tbl%'.'%dbOptions().fileTypeColumnName()%" AS ftype";
		if(dbOptions().isUseMetaColumn()) cols += ", "%top_tbl%'.'%COL_META%" AS meta";
		else cols += ", '' AS meta";
		if(pinode == 0) qs = "SELECT "%cols%" FROM (SELECT 0 AS id) AS t0"; /// fake root row with id == 0
		else qs = "SELECT "%cols%" FROM "%tableName()%" AS t0";
		for(int i=0; i<path.count(); i++) {
			QString p = path.value(i);
			QString t0 = 't' % QString::number(i);
			QString t1 = 't' % QString::number(i+1);
			if(p == ".") { }
			else if(p == "..") {
				qs += " JOIN "%tableName()%" AS "%t1%" ON "%t1%"."%COL_INODE%"="%t0%"."%COL_PINODE;
			}
			else {
				qs += " JOIN "%tableName()%" AS "%t1%" ON "%t1%"."%COL_PINODE%"="%t0%"."%COL_INODE%" AND "%t1%"."%dbOptions().fileNameColumnName()%"="SARG(p);
			}
		}
		qs += " WHERE t0."%COL_INODE%"="IARG(pinode);
		QFSqlQuery q(sqlConnection());
		qfTrash() << qs;
		q.exec(qs);
		if(q.next()) {
			fi.setDbId(q.value("id").toInt());
			fi.setParentDbId(q.value("pid").toInt());
			fi.setType(q.value("ftype").toString());
			fi.setMeta(QFJson::stringToVariant(q.value("meta").toString()));
			QString abs_path = dbFsPathPrefix() % tableName() % dbOptions().tableSeparator() % path.join(dbOptions().pathSeparator());
			fi.setAbsPath(abs_path);
		}
		else {
			//qfWarning() << "QFDbFs::pathToId() ERROR - table:" << tableName() << "parent id:" << pinode << "path:" << path.join("/") << "not found.";
			fi.setDbId(-1);
		}
		f_pathToIdCache[cache_key] = fi;
	}
	if(p_dbfs_attrs)
		*p_dbfs_attrs = fi;
	return fi.dbId();
}
