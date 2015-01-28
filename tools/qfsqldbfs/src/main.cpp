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

static qfs::DbFsDriver *dbfsdrv()
{
	QF_ASSERT_EX(pDbFsDrv != nullptr, "Driver is not set!");
	return pDbFsDrv;
}

static bool isSnapshotReadOnly()
{
	bool read_only = dbfsdrv()->snapshotNumber() < dbfsdrv()->latestSnapshotNumber();
	return read_only;
}

/// inspired by http://www.cs.nmsu.edu/~pfeiffer/fuse-tutorial/

/** Get file attributes.
 *
 * Similar to stat().  The 'st_dev' and 'st_blksize' fields are
 * ignored.  The 'st_ino' field is ignored except if the 'use_ino'
 * mount option is given.
 */
static int qfsqldbfs_getattr(const char *path, struct stat *stbuf)
{
	//qfWarning() << "qfsqldbfs_getattr" << path;
	int res = 0;

	QString spath = QString::fromUtf8(path);
	qfs::DbFsAttrs attrs = dbfsdrv()->attributes(spath);
	bool read_only = isSnapshotReadOnly();

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
		stbuf->st_mtime = attrs.mtime().toTime_t();
	}
	else if(attrs.type() == qfs::DbFsAttrs::File) {
		if(read_only)
			stbuf->st_mode = S_IFREG | 0444;
		else
			stbuf->st_mode = S_IFREG | 0666;
		stbuf->st_nlink = 1;
		stbuf->st_size = attrs.size();
		stbuf->st_mtime = attrs.mtime().toTime_t();
	}
	else {
		res = -ENOENT;
	}
	return res;
}

/** Read directory
 *
 * This supersedes the old getdir() interface.  New applications
 * should use this.
 *
 * The filesystem may choose between two modes of operation:
 *
 * 1) The readdir implementation ignores the offset parameter, and
 * passes zero to the filler function's offset.  The filler
 * function will not return '1' (unless an error happens), so the
 * whole directory is read in a single readdir operation.  This
 * works just like the old getdir() method.
 *
 * 2) The readdir implementation keeps track of the offsets of the
 * directory entries.  It uses the offset parameter and always
 * passes non-zero offset to the filler function.  When the buffer
 * is full (or an error happens) the filler function will return
 * '1'.
 *
 * Introduced in version 2.3
 */
static int qfsqldbfs_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi)
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
	return res;
}

static QMap<QString, QByteArray> s_openFiles;

/** File open operation
 *
 * No creation, or truncation flags (O_CREAT, O_EXCL, O_TRUNC)
 * will be passed to open().  Open should check if the operation
 * is permitted for the given flags.  Optionally open may also
 * return an arbitrary filehandle in the fuse_file_info structure,
 * which will be passed to all file operations.
 *
 * Changed in version 2.2
 */
static int qfsqldbfs_open(const char *path, struct fuse_file_info *fi)
{
	int res = 0;
	bool read_only = isSnapshotReadOnly();
	QString spath = QString::fromUtf8(path);
	qfs::DbFsAttrs attrs = dbfsdrv()->attributes(spath);
	if(attrs.isNull())
		return -ENOENT;

	if(read_only) {
		if ((fi->flags & 3) != O_RDONLY)
			return -EACCES;
	}
	return res;
}

/** Read data from an open file
 *
 * Read should return exactly the number of bytes requested except
 * on EOF or error, otherwise the rest of the data will be
 * substituted with zeroes.  An exception to this is when the
 * 'direct_io' mount option is specified, in which case the return
 * value of the read system call will reflect the return value of
 * this operation.
 *
 * Changed in version 2.2
 */
// I don't fully understand the documentation above -- it doesn't
// match the documentation for the read() system call which says it
// can return with anything up to the amount of data requested. nor
// with the fusexmp code which returns the amount of data also
// returned by read.
static int qfsqldbfs_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{
	Q_UNUSED(fi);

	QString spath = QString::fromUtf8(path);
	if(!s_openFiles.contains(spath)) {
		bool ok;
		QByteArray ba = dbfsdrv()->get(spath, &ok);
		if(!ok)
			return -EBADF;
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

/** Write data to an open file
 *
 * Write should return exactly the number of bytes requested
 * except on error.  An exception to this is when the 'direct_io'
 * mount option is specified (see read operation).
 *
 * Changed in version 2.2
 */
// As  with read(), the documentation above is inconsistent with the
// documentation for the write() system call.
static int qfsqldbfs_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{
	if(isSnapshotReadOnly())
		return -EPERM;

	QString spath = QString::fromUtf8(path);

	QByteArray &ba = s_openFiles[spath];
	size_t len = size + offset;
	if(ba.size() < len)
		ba.resize(len);
	char *data = ba.data();
	memcpy(data + offset, buf, size);

	return size;
}

/** Possibly flush cached data
 *
 * BIG NOTE: This is not equivalent to fsync().  It's not a
 * request to sync dirty data.
 *
 * Flush is called on each close() of a file descriptor.  So if a
 * filesystem wants to return write errors in close() and the file
 * has cached dirty data, this is a good place to write back data
 * and return any errors.  Since many applications ignore close()
 * errors this is not always useful.
 *
 * NOTE: The flush() method may be called more than once for each
 * open().  This happens if more than one file descriptor refers
 * to an opened file due to dup(), dup2() or fork() calls.  It is
 * not possible to determine if a flush is final, so each flush
 * should be treated equally.  Multiple write-flush sequences are
 * relatively rare, so this shouldn't be a problem.
 *
 * Filesystems shouldn't assume that flush will always be called
 * after some writes, or that if will be called at all.
 *
 * Changed in version 2.2
 */
static int qfsqldbfs_flush(const char *path, struct fuse_file_info *fi)
{
	if(isSnapshotReadOnly())
		return -EPERM;

	QString spath = QString::fromUtf8(path);
	if(!s_openFiles.contains(spath)) {
		return -EBADF;
	}
	QByteArray ba = s_openFiles.value(spath);
	bool ok = dbfsdrv()->put(path, ba);
	if(!ok) {
		return -EFAULT;
	}

	return 0;
}

/** Create a file node
 *
 * There is no create() operation, mknod() will be called for
 * creation of all non-directory, non-symlink nodes.
 */
// shouldn't that comment be "if" there is no.... ?
static int qfsqldbfs_mknod(const char *path, mode_t mode, dev_t dev)
{
	if(isSnapshotReadOnly())
		return -EPERM;

	QString spath = QString::fromUtf8(path);
	QPair<QString, QString> pf = qfs::DbFsDriver::splitPathFile(spath);
	bool ok = dbfsdrv()->mkdir(pf.first, qfs::DbFsDriver::O_RECURSIVE);
	if(!ok) {
		return -EFAULT; // pathname points outside your accessible address space.
	}
	ok = dbfsdrv()->mknod(spath);
	if(!ok) {
		return -EEXIST; // pathname already exists.  This includes the case where pathname is a symbolic link, dangling or not.
	}
	return 0;
}

/** Create a directory */
static int qfsqldbfs_mkdir(const char *path, mode_t mode)
{
	if(isSnapshotReadOnly())
		return -EPERM;

	QString spath = QString::fromUtf8(path);
	bool ok = dbfsdrv()->mkdir(spath, qfs::DbFsDriver::O_RECURSIVE);
	if(!ok) {
		return -EFAULT; // pathname points outside your accessible address space.
	}
	return 0;
}

/** Remove a file */
static int qfsqldbfs_unlink(const char *path)
{
	if(isSnapshotReadOnly())
		return -EPERM;

	QString spath = QString::fromUtf8(path);
	bool ok = dbfsdrv()->rmnod(spath);
	if(!ok) {
		return -ENOENT; // A component in pathname does not exist or is a dangling symbolic link, or pathname is empty.
	}
	return 0;
}

/** Remove a directory */
static int qfsqldbfs_rmdir(const char *path)
{
	if(isSnapshotReadOnly())
		return -EPERM;

	QString spath = QString::fromUtf8(path);
	bool ok = dbfsdrv()->rmdir(spath, qfs::DbFsDriver::O_RECURSIVE);
	if(!ok) {
		return -EFAULT;
	}
	return 0;
}

static struct fuse_operations qfsqldbfs_oper = {
	.getattr = qfsqldbfs_getattr,
	.readdir = qfsqldbfs_readdir,
	.open = qfsqldbfs_open,
	.read = qfsqldbfs_read,
	.write = qfsqldbfs_write,
	.flush = qfsqldbfs_flush,
	.mknod = qfsqldbfs_mknod,
	.mkdir = qfsqldbfs_mkdir,
	.unlink = qfsqldbfs_unlink,
	.rmdir = qfsqldbfs_rmdir,
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

	qfDebug() << "snapshotNumber:" << dbfsdrv()->snapshotNumber() << "latestSnapshotNumber:" << dbfsdrv()->latestSnapshotNumber();

	int fuse_argc = args.length();
	if(dbfs_options_index > 0)
		fuse_argc = dbfs_options_index;
	int ret = fuse_main(fuse_argc, argv, &qfsqldbfs_oper, NULL);

	delete pDbFsDrv;
	return ret;
}
