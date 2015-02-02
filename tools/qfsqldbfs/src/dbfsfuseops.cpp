#include "dbfsfuseops.h"

#include <qf/core/sql/dbfsdriver.h>
#include <qf/core/log.h>

#include <unistd.h>

namespace qfs = qf::core::sql;

struct OpenFile
{
	qfs::DbFsAttrs attrs;
	QByteArray data;
	bool dataLoaded = false;
	bool dataDirty = false;
};

static QMap<QString, OpenFile> s_openFiles;
static qfs::DbFsDriver *pDbFsDrv = nullptr;

static qfs::DbFsDriver *dbfsdrv()
{
	QF_ASSERT_EX(pDbFsDrv != nullptr, "Driver is not set!");
	return pDbFsDrv;
}

static bool isSnapshotReadOnly()
{
	return dbfsdrv()->isSnapshotReadOnly();
}

static QIODevice::OpenMode openModeFromPosix(int posix_flags)
{
	QIODevice::OpenMode ret;
	if ((posix_flags & O_ACCMODE) == O_RDONLY){
		ret |= QIODevice::ReadOnly;
	}
	if ((posix_flags & O_ACCMODE) == O_WRONLY){
		ret |= QIODevice::WriteOnly;
	}
	if ((posix_flags & O_ACCMODE) == O_RDWR){
		ret |= QIODevice::ReadWrite;
	}
	if (posix_flags & O_APPEND){
		ret |= QIODevice::Append;
	}
	if (posix_flags & O_TRUNC){
		ret |= QIODevice::Truncate;
	}
	return ret;
}

static QString openModeToString(QIODevice::OpenMode mode)
{
	QString ret;
	if(mode & QIODevice::ReadWrite)
		ret += "READ_WRITE";
	else if(mode & QIODevice::ReadOnly)
		ret += "READ_ONLY";
	else if(mode & QIODevice::WriteOnly)
		ret += "WRITE_ONLY";

	if(mode & QIODevice::Append)
		ret += "+APPEND";
	if(mode & QIODevice::Truncate)
		ret += "+TRUNCATE";
	return ret;
}

void qfsqldbfs_setdriver(qf::core::sql::DbFsDriver *drv)
{
	pDbFsDrv = drv;
}

/// inspired by http://www.cs.nmsu.edu/~pfeiffer/fuse-tutorial/
/// http://websvn.kde.org/trunk/playground/libs/kiofuse/

/** Get file attributes.
 *
 * Similar to stat().  The 'st_dev' and 'st_blksize' fields are
 * ignored.  The 'st_ino' field is ignored except if the 'use_ino'
 * mount option is given.
 */
int qfsqldbfs_getattr(const char *path, struct stat *stbuf)
{
	qfLogFuncFrame() << path;
	int res = 0;

	QString spath = QString::fromUtf8(path);
	qfs::DbFsAttrs attrs = dbfsdrv()->attributes(spath);
	bool read_only = isSnapshotReadOnly();

	memset(stbuf, 0, sizeof(struct stat));
	stbuf->st_mtime = attrs.mtime().toTime_t();
	stbuf->st_uid = getuid();
	stbuf->st_gid = getgid();

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
	qfDebug() << "\t ret:" << res;
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
int qfsqldbfs_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi)
{
	qfLogFuncFrame() << path;
	(void) offset;
	(void) fi;

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
	qfDebug() << "\t ret OK";
	return 0;
}

static int loadData(const QString &spath)
{
	qfLogFuncFrame() << spath;
	if(!s_openFiles.contains(spath)) {
		qfDebug() << "File is not open!";
		return -EBADF;
	}
	OpenFile &of = s_openFiles[spath];
	if(!of.dataLoaded) {
		bool ok;
		QByteArray ba = dbfsdrv()->get(spath, &ok);
		if(!ok)
			return -EFAULT;
		of.data = ba;
		of.dataLoaded = true;
	}
	return of.data.size();
}

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
int qfsqldbfs_open(const char *path, struct fuse_file_info *fi)
{
	QIODevice::OpenMode mode = openModeFromPosix(fi->flags);
	qfLogFuncFrame() << path << "in mode:" << openModeToString(mode);
	int ret = 0;
	do {
		QString spath = QString::fromUtf8(path);
		qfs::DbFsAttrs attrs = dbfsdrv()->attributes(spath);
		if(attrs.isNull()) {
			qfDebug() << "Cannot get file attributes!";
			ret = -ENOENT;
			break;
		}
		bool snapshot_read_only = isSnapshotReadOnly();
		if(snapshot_read_only) {
			if (mode != QIODevice::ReadOnly) {
				qfDebug() << "Cannot open readonly file in mode:" << openModeToString(mode);
				ret = -EACCES;
				break;
			}
		}
		if(mode & QIODevice::Append) {
			int size = loadData(spath);
			if(size < 0) {
				ret = -EFAULT;
				break;
			}
		}
		OpenFile &of = s_openFiles[spath];
		of.attrs = attrs;
	} while(false);
	qfDebug() << "\t ret:" << ret;
	return ret;
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
int qfsqldbfs_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{
	qfLogFuncFrame() << path;
	Q_UNUSED(fi);

	QString spath = QString::fromUtf8(path);
	if(!s_openFiles.contains(spath)) {
		qfDebug() << "File is not open!";
		return -EBADF;
	}
	int dsize = loadData(spath);
	if(dsize < 0) {
		return -EFAULT;
	}
	OpenFile &of = s_openFiles[spath];
	const QByteArray &ba = of.data;
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
	qfDebug() << "\t ret size:" << size;
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
int qfsqldbfs_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{
	qfLogFuncFrame() << path;
	Q_UNUSED(fi)
	if(isSnapshotReadOnly())
		return -EPERM;

	QString spath = QString::fromUtf8(path);
	if(!s_openFiles.contains(spath)) {
		qfDebug() << "File is not open!";
		return -EBADF;
	}
	OpenFile &of = s_openFiles[spath];
	of.dataDirty = true;
	QByteArray &ba = of.data;
	int len = (int)size + offset;
	if(ba.size() < len)
		ba.resize(len);
	char *data = ba.data();
	memcpy(data + offset, buf, size);
	qfDebug() << "\t ret size:" << size;
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
int qfsqldbfs_flush(const char *path, struct fuse_file_info *fi)
{
	qfLogFuncFrame() << path;
	Q_UNUSED(fi)
	int ret = 0;
	do {
		if(isSnapshotReadOnly()) {
			qfDebug() << "snapshot RO";
			ret = -EPERM;
			break;
		}
		QString spath = QString::fromUtf8(path);
		if(!s_openFiles.contains(spath)) {
			qfDebug() << "File is not open!";
			ret = -EBADF;
			break;
		}
		OpenFile &of = s_openFiles[spath];

		if(of.dataDirty) {
			const QByteArray &ba = of.data;
			bool ok = dbfsdrv()->put(path, ba);
			if(!ok) {
				qfDebug() << "DBFS PUT error";
				ret = -EFAULT;
				break;
			}
			of.dataDirty = false;
		}
	} while(false);
	qfDebug() << "\t ret:" << ret;
	return ret;
}

/** Create a file node
 *
 * There is no create() operation, mknod() will be called for
 * creation of all non-directory, non-symlink nodes.
 */
// shouldn't that comment be "if" there is no.... ?
int qfsqldbfs_mknod(const char *path, mode_t mode, dev_t dev)
{
	qfLogFuncFrame() << path;
	Q_UNUSED(mode)
	Q_UNUSED(dev)
	if(isSnapshotReadOnly())
		return -EPERM;

	QString spath = QString::fromUtf8(path);
	QPair<QString, QString> pf = qfs::DbFsDriver::splitPathFile(spath);
	bool ok = dbfsdrv()->mkfile(spath);
	if(!ok) {
		return -EEXIST; // pathname already exists.  This includes the case where pathname is a symbolic link, dangling or not.
	}
	qfDebug() << "\t ret OK";
	return 0;
}

/** Create a directory */
int qfsqldbfs_mkdir(const char *path, mode_t mode)
{
	qfLogFuncFrame() << path;
	Q_UNUSED(mode)
	if(isSnapshotReadOnly())
		return -EPERM;

	QString spath = QString::fromUtf8(path);
	bool ok = dbfsdrv()->mkdir(spath);
	if(!ok) {
		return -EFAULT; // pathname points outside your accessible address space.
	}
	qfDebug() << "\t ret OK";
	return 0;
}

/** Remove a file */
int qfsqldbfs_unlink(const char *path)
{
	qfLogFuncFrame() << path;
	if(isSnapshotReadOnly())
		return -EPERM;

	QString spath = QString::fromUtf8(path);
	bool ok = dbfsdrv()->rmnod(spath);
	if(!ok) {
		return -ENOENT; // A component in pathname does not exist or is a dangling symbolic link, or pathname is empty.
	}
	qfDebug() << "\t ret OK";
	return 0;
}

/** Remove a directory */
int qfsqldbfs_rmdir(const char *path)
{
	qfLogFuncFrame() << path;
	if(isSnapshotReadOnly())
		return -EPERM;

	QString spath = QString::fromUtf8(path);
	bool ok = dbfsdrv()->rmnod(spath);
	if(!ok) {
		return -EFAULT;
	}
	qfDebug() << "\t ret OK";
	return 0;
}

int qfsqldbfs_utime(const char *path, utimbuf *ubuf)
{
	qfLogFuncFrame() << path;
	/// dbfs handles mtime on its own
	Q_UNUSED(ubuf)
	return 0;
}

/** Release an open file
 *
 * Release is called when there are no more references to an open
 * file: all file descriptors are closed and all memory mappings
 * are unmapped.
 *
 * For every open() call there will be exactly one release() call
 * with the same flags and file descriptor.  It is possible to
 * have a file opened more than once, in which case only the last
 * release will mean, that no more reads/writes will happen on the
 * file.  The return value of release is ignored.
 *
 * Changed in version 2.2
 */
int qfsqldbfs_release(const char *path, fuse_file_info *fi)
{
	qfLogFuncFrame() << path;
	Q_UNUSED(fi)
	int ret = 0;
	QString spath = QString::fromUtf8(path);
	do {
		if(!s_openFiles.contains(spath)) {
			qfDebug() << "File is not open!";
			ret = -EBADF;
			break;
		}
		s_openFiles.remove(spath);
	} while(false);
	return ret;
}

/** Change the size of a file */
int qfsqldbfs_truncate(const char *path, off_t newsize)
{
	qfLogFuncFrame() << path;
	int ret = 0;
	QString spath = QString::fromUtf8(path);
	do {
		if(!s_openFiles.contains(spath)) {
			qfDebug() << "File is not open!";
			ret = -EBADF;
			break;
		}
		OpenFile &of = s_openFiles[spath];
		of.dataDirty = true;
		of.dataLoaded = true;
		of.data.resize(newsize);
	} while(false);
	return ret;
}
