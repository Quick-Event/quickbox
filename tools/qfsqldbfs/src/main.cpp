#define FUSE_USE_VERSION 26

#include <qf/core/sql/dbfsdriver.h>
#include <qf/core/sql/dbfsattrs.h>
#include <qf/core/utils/table.h>
#include <qf/core/log.h>
#include <qf/core/logdevice.h>

#include <QSqlDatabase>
#include <QSqlError>

#include <fuse.h>
#include <iostream>
#include <limits>

namespace qfs = qf::core::sql;

static qfs::DbFsDriver *pDbFsDrv = nullptr;
static int s_latestSnapshotNumber = std::numeric_limits<int32_t>::max();

static qfs::DbFsDriver *dbfsdrv()
{
	QF_ASSERT_EX(pDbFsDrv != nullptr, "Driver is not set!");
	return pDbFsDrv;
}

static int qfsqldbfs_getattr(const char *path, struct stat *stbuf)
{
	//qfWarning() << "qfsqldbfs_getattr" << path;
	int res = 0;

	QString spath = QString::fromUtf8(path);
	qfs::DbFsAttrs attrs = dbfsdrv()->attributes(spath);
	bool read_only = dbfsdrv()->snapshotNumber() < s_latestSnapshotNumber;

	memset(stbuf, 0, sizeof(struct stat));
	if(attrs.isNull()) {
		res = -ENOENT;
	}
	else if(attrs.type() == qfs::DbFsAttrs::Dir) {
		if(read_only)
			stbuf->st_mode = S_IFDIR | 0555;
		else
			stbuf->st_mode = S_IFDIR | 0777;
		stbuf->st_nlink = 2; /// all directories have at least 2 links, itself, and the link back to
	}
	else if(attrs.type() == qfs::DbFsAttrs::File) {
		if(read_only)
			stbuf->st_mode = S_IFREG | 0444;
		else
			stbuf->st_mode = S_IFREG | 0666;
		stbuf->st_nlink = 1;
		stbuf->st_size = attrs.size();
	}
	else {
		res = -ENOENT;
	}
	/*
	if (strcmp(path, "/") == 0) {
		stbuf->st_mode = S_IFDIR | 0755;
		stbuf->st_nlink = 2; /// all directories have at least 2 links, itself, and the link back to
	} else if (strcmp(path, qfsqldbfs_path) == 0) {
		stbuf->st_mode = S_IFREG | 0444;
		stbuf->st_nlink = 1;
		stbuf->st_size = strlen(qfsqldbfs_str);
	} else
		res = -ENOENT;
*/
	return res;
}

static int qfsqldbfs_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
							 off_t offset, struct fuse_file_info *fi)
{
	//qfWarning() << "qfsqldbfs_readdir" << path;
	(void) offset;
	(void) fi;

	int res = 0;

	filler(buf, ".", NULL, 0);
	filler(buf, "..", NULL, 0);
	QString spath = QString::fromUtf8(path);
	QList<qfs::DbFsAttrs> attrs = dbfsdrv()->childAttributes(spath);
	for(qfs::DbFsAttrs attr : attrs) {
		QByteArray ba = attr.name().toUtf8();
		const char *pname = ba.constData();
		//qfWarning() << "\t" << pname;
		filler(buf, pname, NULL, 0);
	}
	/*
	if (strcmp(path, "/") != 0)
		return -ENOENT;

	filler(buf, qfsqldbfs_path + 1, NULL, 0);
*/
	return res;
}

static QMap<QString, QByteArray> s_openFiles;

static int qfsqldbfs_open(const char *path, struct fuse_file_info *fi)
{
	int res = 0;
	QString spath = QString::fromUtf8(path);

	qfs::DbFsAttrs attrs = dbfsdrv()->attributes(spath);
	if(attrs.isNull())
		return -ENOENT;

	if ((fi->flags & 3) != O_RDONLY)
		return -EACCES;

	/*
	if (strcmp(path, qfsqldbfs_path) != 0)
		return -ENOENT;

*/
	return res;
}

static int qfsqldbfs_read(const char *path, char *buf, size_t size, off_t offset,
						  struct fuse_file_info *fi)
{
	Q_UNUSED(fi);

	QString spath = QString::fromUtf8(path);
	if(!s_openFiles.contains(spath)) {
		bool ok;
		QByteArray ba = dbfsdrv()->get(spath, &ok);
		if(!ok)
			return EBADF;
		s_openFiles[spath] = ba;
	}
	QByteArray ba = s_openFiles.value(spath);
	const char *data = ba.constData();
	size_t len = ba.size();
	size_t uoffset = (size_t)offset;
	if (uoffset < len) {
		if (uoffset + size > len)
			size = len - offset;
		memcpy(buf, data + uoffset, size);
	}
	else {
		size = 0;
	}
	return size;
}

static struct fuse_operations qfsqldbfs_oper = {
	.getattr	= qfsqldbfs_getattr,
	.readdir	= qfsqldbfs_readdir,
	.open		= qfsqldbfs_open,
	.read		= qfsqldbfs_read,
};

int main(int argc, char *argv[])
{
	QScopedPointer<qf::core::LogDevice> file_log_device(qf::core::FileLogDevice::install());
	QStringList args = file_log_device->setDomainTresholds(argc, argv);
	file_log_device->setPrettyDomain(true);

	QString o_database;
	QString o_user;
	QString o_password;
	QString o_host;
	int o_port = 0;
	QString o_table_name;
	int o_snapshot = -1;
	bool o_create_db = false;
	bool o_list_snapshots = false;
	bool o_ask_passwd = false;

	int dbfs_options_index = args.indexOf(QStringLiteral("--dbfs"));
	if(dbfs_options_index > 0) {
		for(int i=dbfs_options_index+1; i<args.count(); i++) {
			QString arg = args[i];
			if(arg == QStringLiteral("--host")) {
				if(i<args.count()-1) {
					i++;
					o_host = args[i];
				}
			}
			else if(arg == QStringLiteral("--port")) {
				if(i<args.count()-1) {
					i++;
					o_port = args[i].toInt();
				}
			}
			else if(arg == QStringLiteral("-u") || arg == QStringLiteral("--user")) {
				if(i<args.count()-1) {
					i++;
					o_user = args[i];
				}
			}
			else if(arg == QStringLiteral("-p") || arg == QStringLiteral("--password")) {
				if(i<args.count()-1) {
					QString p = args[i+1];
					if(p.startsWith('-')) {
						o_ask_passwd = true;
					}
					else {
						o_password = p;
						i++;
					}
				}
			}
			else if(arg == QStringLiteral("--database")) {
				if(i<args.count()-1) {
					i++;
					o_database = args[i];
				}
			}
			else if(arg == QStringLiteral("--table-name")) {
				if(i<args.count()-1) {
					i++;
					o_table_name = args[i];
				}
			}
			else if(arg == QStringLiteral("--snapshot")) {
				if(i<args.count()-1) {
					i++;
					o_snapshot = args[i].toInt();
				}
			}
			else if(arg == QStringLiteral("--create")) {
				o_create_db = true;
			}
			else if(arg == QStringLiteral("--list-snapshots")) {
				o_list_snapshots = true;
			}
			else if(arg == QStringLiteral("-h") || arg == QStringLiteral("--help")) {
				std::cout << argv[0] << "FUSE_options --dbfs DBFS_options" << std::endl;
				std::cout << "FUSE_options" << std::endl;
				std::cout << "\tuse -h switch to print FUSE options" << std::endl;
				std::cout << "DBFS_options" << std::endl;
				std::cout << "\t--dbfs\t" << "DBFS options separator, all options prior this will be ignored by DBFS" << std::endl;
				std::cout << "\t--host <host>\t" << "Database host" << std::endl;
				std::cout << "\t--port <port>\t" << "Database port" << std::endl;
				std::cout << "\t--u" << std::endl;
				std::cout << "\t--user <user>\t" << "Database user" << std::endl;
				std::cout << "\t--p" << std::endl;
				std::cout << "\t--password [<password>]\t" << "Database user password" << std::endl;
				std::cout << "\t--database <database>\t" << "Database name" << std::endl;
				std::cout << "\t--snapshot <snapshot_number>\t" << "Mount snapshot snapshot_number (read only)" << std::endl;
				std::cout << "\t--table-name\t" << "DBFS table name" << std::endl;
				std::cout << "\t--create\t" << "Create DBFS tables" << std::endl;
				std::cout << "\t--create\t" << "Create DBFS tables" << std::endl;
				std::cout << "\t--list-snapshots\t" << "List DBFS snapshots" << std::endl;
				exit(0);
			}
		}
	}

	if(o_ask_passwd) {
		char pwd[256];
		std::cout << "Please, enter your password: ";
		std::cin.getline (pwd, 256);
		o_password = QString::fromUtf8(pwd);
	}

	QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL");
	db.setHostName(o_host);
	if(o_port > 0)
		db.setPort(o_port);
	db.setDatabaseName(o_database);
	db.setUserName(o_user);
	bool ok = db.open();
	if(!ok) {
		qfError() << db.lastError().text();
		exit(1);
	}
	pDbFsDrv = new qfs::DbFsDriver();
	pDbFsDrv->setConnectionName(db.connectionName());

	if(!o_table_name.isEmpty()) {
		dbfsdrv()->setTableName(o_table_name);
	}
	if(o_create_db) {
		if(!dbfsdrv()->createDbFs()) {
			qfError() << "Error creating dbfs table" << dbfsdrv()->tableName();
		}
		exit(1);
	}
	if(o_list_snapshots) {
		qf::core::utils::Table t = dbfsdrv()->listSnapshots();
		QString s = t.toString();
		std::cout << qPrintable(s) << std::endl;
		exit(0);
	}
	if(o_snapshot >= 0) {
		dbfsdrv()->setSnapshotNumber(o_snapshot);
	}

	s_latestSnapshotNumber = dbfsdrv()->latestSnapshotNumber();

	qfDebug() << "snapshotNumber:" << dbfsdrv()->snapshotNumber() << "latestSnapshotNumber:" << s_latestSnapshotNumber;

	int fuse_argc = args.length();
	if(dbfs_options_index > 0)
		fuse_argc = dbfs_options_index;
	int ret = fuse_main(fuse_argc, argv, &qfsqldbfs_oper, NULL);

	delete pDbFsDrv;
	return ret;
}
