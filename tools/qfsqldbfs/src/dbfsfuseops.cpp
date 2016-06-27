#include "dbfsfuseops.h"
#include "openfile.h"

#include <qf/core/sql/dbfsdriver.h>
#include <qf/core/log.h>
#include <qf/core/assert.h>

#include <QMutex>
#include <QMutexLocker>

#include <unistd.h>

namespace qfs = qf::core::sql;

typedef QSet<uint64_t> OpenHandles;

static QMap<uint64_t, OpenFile> s_openFilesForHandle;
static QMap<QString, OpenHandles> s_openHandlesForFile;
static qfs::DbFsDriver *pDbFsDrv = nullptr;

//#define USE_MUTEX_LOCKING
#ifdef USE_MUTEX_LOCKING
static QMutex s_apiMutex;
#define MUTEX_LOCKER QMutexLocker locker(&s_apiMutex)
#else
#define MUTEX_LOCKER
#endif

static OpenFile openFile(uint64_t handle)
{
	return s_openFilesForHandle.value(handle);
}

static void setOpenFile(uint64_t handle, const OpenFile &of)
{
	if(of.isNull())
		s_openFilesForHandle.remove(handle);
	else
		s_openFilesForHandle[handle] = of;
}

static OpenHandles openFileHandles(const QString &spath)
{
	return s_openHandlesForFile.value(spath);
}

static void setOpenFileHandles(const QString &spath, const OpenHandles &handles)
{
	if(handles.isEmpty())
		s_openHandlesForFile.remove(spath);
	else
		s_openHandlesForFile[spath] = handles;
}

static qfs::DbFsDriver *dbfsdrv()
{
	QF_ASSERT_EX(pDbFsDrv != nullptr, "Driver is not set!");
	return pDbFsDrv;
}

static uint64_t nextFileHandle()
{
	static uint64_t n = 0;
	return ++n;
}

static const int QIODevice_Create = 0x100;

static QIODevice::OpenMode openModeFromPosix(int posix_flags)
{
	int ret = 0;
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
	if (posix_flags & O_CREAT){
		ret |= QIODevice_Create;
	}
	return (QIODevice::OpenMode)ret;
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
	if(mode & QIODevice_Create)
		ret += "+CREATE";
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
	MUTEX_LOCKER;
	int res = 0;

	QString spath = QString::fromUtf8(path);
	qfs::DbFsAttrs attrs = dbfsdrv()->attributes(spath);
	bool read_only = false;

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
	qfLogFuncFrame() << path << "file handle:" << fi->fh;
	Q_UNUSED(offset)
	//(void) fi;
	MUTEX_LOCKER;

	filler(buf, ".", NULL, 0);
	filler(buf, "..", NULL, 0);
	QString spath = QString::fromUtf8(path);
	QList<qfs::DbFsAttrs> attrs = dbfsdrv()->childAttributes(spath);
	Q_FOREACH(qfs::DbFsAttrs attr, attrs) {
		QByteArray ba = attr.name().toUtf8();
		const char *pname = ba.constData();
		//qfWarning() << "\t" << pname;
		filler(buf, pname, NULL, 0);
	}
	qfDebug() << "\t ret OK";
	return 0;
}

static int loadData(const QString &spath, OpenFile &of)
{
	qfLogFuncFrame() << spath;
	bool ok;
	QByteArray ba = dbfsdrv()->get(spath, &ok);
	if(!ok) {
		return -EFAULT;
	}
	else {
		of.setData(ba);
		of.setDataLoaded(true);
	}
	return ok;
}

static int qfsqldbfs_open_common(const char *path, mode_t mode, struct fuse_file_info *fi)
{
	Q_UNUSED(mode);
	QIODevice::OpenMode om = openModeFromPosix(fi->flags);
	qfLogFuncFrame() << path << "in mode:" << openModeToString(om);
	MUTEX_LOCKER;
	int ret = 0;
	do {
		QString spath = QString::fromUtf8(path);
		qfs::DbFsAttrs attrs = dbfsdrv()->attributes(spath);
		if(attrs.isNull()) {
			if(om && QIODevice_Create) {
				bool ok = dbfsdrv()->mkfile(spath);
				if(!ok) {
					ret = -EEXIST;
					break;
				}
			}
			else {
				qfWarning() << spath << "Cannot get file attributes!";
				ret = -ENOENT;
				break;
			}
		}
		uint64_t handle = nextFileHandle();
		OpenFile of = openFile(handle);
		of.setAttrs(attrs);
		of.setOpenMode(om);
		setOpenFile(handle, of);
		fi->fh = handle;
		OpenHandles hs = openFileHandles(spath);
		hs << handle;
		setOpenFileHandles(spath, hs);
		qfDebug() << "\t !!!!!!!!!!!!! open file handle:" << fi->fh << "path:" << path;
	} while(false);
	qfDebug() << "\t ret:" << ret;
	return ret;
}

/** Create and open a file
 *
 * If the file does not exist, first create it with the specified mode, and then open it.
 * If this method is not implemented or under Linux kernel versions earlier than 2.6.15,
 * the mknod() and open() methods will be called instead.
 *
 * Introduced in version 2.5
 */
int qfsqldbfs_create(const char *path, mode_t mode, struct fuse_file_info *fi)
{
	QIODevice::OpenMode om = openModeFromPosix(mode);
	qfLogFuncFrame() << path << "in mode:" << openModeToString(om);
	return qfsqldbfs_open_common(path, mode, fi);
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
	return qfsqldbfs_open_common(path, 0, fi);
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
	qfLogFuncFrame() << path << "file handle:" << fi->fh;
	//Q_UNUSED(fi);
	MUTEX_LOCKER;

	QString spath = QString::fromUtf8(path);
	uint64_t handle = fi->fh;
	OpenFile of = openFile(handle);
	if(of.isNull()) {
		qfWarning() << spath << "handle:" << handle << "File is not open!";
		return -EBADF;
	}
	if(!of.isDataLoaded()) {
		if(!loadData(spath, of)) {
			qfWarning() << spath << "handle:" << handle << "Error load data";
			return -EFAULT;
		}
	}
	QByteArray ba = of.data();
	qfDebug() << "reading" << size << "of data at offset" << offset << ", data size:" << ba.size();
	const char *data = ba.constData();
	size_t len = ba.size();
	size_t uoffset = (size_t)offset;
	if (uoffset < len) {
		if (uoffset + size > len)
			size = len - offset;
		//qfInfo() << "offset:" << uoffset << "size:" << size << "data:" << data;
		memcpy(buf, data + uoffset, size);
		//qfInfo() << size << "####" << buf;
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
	qfLogFuncFrame() << path << "file handle:" << fi->fh;
	MUTEX_LOCKER;

	uint64_t handle = fi->fh;
	QString spath = QString::fromUtf8(path);
	OpenFile of = openFile(handle);
	if(of.isNull()) {
		qfWarning() << spath << "handle:" << handle << "File is not open!";
		return -EBADF;
	}
	if(!(of.openMode() & QIODevice::WriteOnly)) {
		qfWarning() << spath << "handle:" << handle << "File is not open!";
		return -EPERM;
	}
	if(!of.isDataLoaded() && (of.openMode() & QIODevice::Append)) {
		if(!loadData(spath, of)) {
			qfWarning() << spath << "handle:" << handle << "Error load data for APPEND";
			return -EFAULT;
		}
	}
	of.setDataLoaded(true);
	of.setDataDirty(true);
	QByteArray &ba = of.dataRef();
	int len = (int)size + offset;
	if(ba.size() < len)
		ba.resize(len);
	char *data = ba.data();
	memcpy(data + offset, buf, size);
	setOpenFile(handle, of);
	qfDebug() << "\t ret size:" << size;
	return size;
}

/** Synchronize file contents
 * If the datasync parameter is non-zero, then only the user data should be flushed, not the meta data.
 *
 * Changed in version 2.2
 */
int qfsqldbfs_fsync(const char *path, int isdatasync, struct fuse_file_info *fi)
{
	qfLogFuncFrame() << path << "file handle:" << fi->fh;
	Q_UNUSED(isdatasync);
	MUTEX_LOCKER;
	int ret = qfsqldbfs_flush(path, fi);
	qfDebug() << "\t ret:" << ret;
	return ret;
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
	qfLogFuncFrame() << path << "file handle:" << fi->fh;
	MUTEX_LOCKER;
	int ret = 0;
	do {
		uint64_t handle = fi->fh;
		QString spath = QString::fromUtf8(path);
		OpenFile of = openFile(handle);
		if(of.isNull()) {
			qfWarning() << spath << "handle:" << handle << "File is not open!";
			ret = -EBADF;
			break;
		}
		if(!(of.openMode() & QIODevice::WriteOnly) && of.isDataDirty()) {
			qfWarning() << spath << "handle:" << handle << "flush in open for read mode but with dirty data!!!";
			break;
		}
		if(of.isDataDirty()) {
			QByteArray ba = of.data();
			qfDebug() << "flushing data on path:" << path << "data:" << ((ba.size() < 100)? ba : ba.mid(100));
			bool ok = dbfsdrv()->put(spath, ba);
			if(!ok) {
				qfWarning() << spath << "handle:" << handle << "DBFS PUT error";
				ret = -EFAULT;
				break;
			}
			of.setDataDirty(false);
			setOpenFile(handle, of);
		}
	} while(false);
	qfDebug() << "\t ret:" << ret;
	return ret;
}

/** Create a file node
 *
 * There is no create() operation, mknod() will be called for
 * creation of all non-directory, non-symlink nodes.
 * If the filesystem defines a create() method, then for regular files that will be called instead.
 */
int qfsqldbfs_mknod(const char *path, mode_t mode, dev_t dev)
{
	QIODevice::OpenMode om = openModeFromPosix(mode);
	qfLogFuncFrame() << path << "in mode:" << openModeToString(om);
	Q_UNUSED(dev)
	MUTEX_LOCKER;

	QString spath = QString::fromUtf8(path);
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
	MUTEX_LOCKER;

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
	MUTEX_LOCKER;

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
	MUTEX_LOCKER;

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
	MUTEX_LOCKER;
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
	qfLogFuncFrame() << path << "file handle:" << fi->fh;
	MUTEX_LOCKER;
	int ret = 0;
	uint64_t handle = fi->fh;
	QString spath = QString::fromUtf8(path);
	do {
		OpenFile of = openFile(handle);
		if(of.isNull()) {
			qfDebug() << "File is not open!";
			ret = -EBADF;
			break;
		}
		setOpenFile(handle, OpenFile());
		OpenHandles handles = openFileHandles(spath);
		handles.remove(handle);
		setOpenFileHandles(spath, handles);
	} while(false);
	return ret;
}

static int truncate_open_handle(const QString &spath, uint64_t handle, int new_size)
{
	qfLogFuncFrame() << "file handle:" << handle;
	int ret = 0;
	do {
		OpenFile of = openFile(handle);
		if(of.isNull()) {
			qfDebug() << "File is not open!";
			ret = -EBADF;
			break;
		}
		if(!of.isDataLoaded()) {
			if(!loadData(spath, of)) {
				qfWarning() << spath << "handle:" << handle << "Error load data";
				ret = -EFAULT;
				break;
			}
		}
		int old_size = of.data().size();
		if(new_size != old_size) {
			of.setDataDirty(true);
			of.dataRef().resize(new_size);
		}
		setOpenFile(handle, of);
	} while(false);
	return ret;
}

/** Change the size of a file */
int qfsqldbfs_truncate(const char *path, off_t new_size)
{
	qfLogFuncFrame() << path;
	MUTEX_LOCKER;
	int ret = 0;

	QString spath = QString::fromUtf8(path);
	OpenHandles handles = openFileHandles(spath);
	Q_FOREACH(uint64_t handle, handles) {
		OpenFile of = openFile(handle);
		if(of.isNull()) {
			if(!dbfsdrv()->truncate(spath, new_size))
				ret = -EFAULT;
		}
		else {
			ret = truncate_open_handle(spath, handle, new_size);
		}
			/*
			 * I was facing a strange bug with truncate when issuing
			 *
			 * echo "foo" > /home/fanda/fuse-mounted-dir/bar.txt
			 *
			 * 1. truncate to 0 lenght
			 * 2. write("foo") was executed
			 *
			 * cat /home/fanda/fuse-mounted-dir/bar.txt
			 *
			 * returned empty file when first time called
			 * proper "foo" value was returned next times
			 *
			 * After 2 days of debugging I have found that:
			 * 1. this is caused by caling dbfsdrv()->put("bar.txt", QByteArray()) when file was truncated
			 * 2. this put is rewriten immediately by dbfsdrv()->put("bar.txt", QByteArray("foo")) when new content is set, so first call is superfluous but woundless
			 * 3. There are more solutions:
			 *     a. remove first put()
			 *     b. call dbfsdrv()->put("bar.txt", QByteArray("any not null and not empty string")) in the truncate phase
			 *        realy strange is that just passing not empty QByteArray to q.bindValue(":data", ba); in sqlUpdate() can solve the problem
			 * 4. qfsqldbfs_read() for cat is called and it returned correct value, but it was from some strange reason ignored
			 * 5. solution a. or b. caused that qfsqldbfs_read() was called twice and second call value was not ignored
			 */
	}
	return ret;
}

/**
 * Change the size of an open file
 *
 * This method is called instead of the truncate() method if the
 * truncation was invoked from an ftruncate() system call.
 *
 * If this method is not implemented or under Linux kernel
 * versions earlier than 2.6.15, the truncate() method will be
 * called instead.
 *
 * Introduced in version 2.5
 */
int qfsqldbfs_ftruncate(const char *path, off_t new_size, struct fuse_file_info *fi)
{
	qfLogFuncFrame() << path << "file handle:" << fi->fh;
	MUTEX_LOCKER;
	QString spath = QString::fromUtf8(path);
	uint64_t handle = fi->fh;
	int ret = truncate_open_handle(spath, handle, new_size);
	return ret;
}

int qfsqldbfs_chmod(const char *path, mode_t mode)
{
	qfLogFuncFrame() << path << "mode:" << mode;
	MUTEX_LOCKER;
	int ret = 0;
	return ret;
}

int qfsqldbfs_chown(const char *path, uid_t uid, gid_t gid)
{
	qfLogFuncFrame() << path << "uid:" << uid << "gid:" << gid;
	MUTEX_LOCKER;
	int ret = 0;
	return ret;
}

/** Rename a file */
// both path and newpath are fs-relative

int qfsqldbfs_rename(const char *path, const char *new_path)
{
	qfLogFuncFrame() << path << "fnew_path:" << new_path;
	MUTEX_LOCKER;

	QString sopath = QString::fromUtf8(path);
	QString snpath = QString::fromUtf8(new_path);
	int ret = dbfsdrv()->rename(sopath, snpath)? 0: -EFAULT;
	return ret;
}

