#include "dbfsfuseops.h"

#include <qf/core/sql/dbfsdriver.h>

namespace qfs = qf::core::sql;

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
int qfsqldbfs_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi)
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
int qfsqldbfs_open(const char *path, struct fuse_file_info *fi)
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
int qfsqldbfs_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
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
int qfsqldbfs_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{
	Q_UNUSED(fi)
	if(isSnapshotReadOnly())
		return -EPERM;

	QString spath = QString::fromUtf8(path);

	QByteArray &ba = s_openFiles[spath];
	int len = (int)size + offset;
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
int qfsqldbfs_flush(const char *path, struct fuse_file_info *fi)
{
	Q_UNUSED(fi)
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
int qfsqldbfs_mknod(const char *path, mode_t mode, dev_t dev)
{
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
	return 0;
}

/** Create a directory */
int qfsqldbfs_mkdir(const char *path, mode_t mode)
{
	Q_UNUSED(mode)
	if(isSnapshotReadOnly())
		return -EPERM;

	QString spath = QString::fromUtf8(path);
	bool ok = dbfsdrv()->mkdir(spath);
	if(!ok) {
		return -EFAULT; // pathname points outside your accessible address space.
	}
	return 0;
}

/** Remove a file */
int qfsqldbfs_unlink(const char *path)
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
int qfsqldbfs_rmdir(const char *path)
{
	if(isSnapshotReadOnly())
		return -EPERM;

	QString spath = QString::fromUtf8(path);
	bool ok = dbfsdrv()->rmnod(spath);
	if(!ok) {
		return -EFAULT;
	}
	return 0;
}



