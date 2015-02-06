#ifndef DBFSFUSEOPS_H
#define DBFSFUSEOPS_H

#include "thisfuse.h"

namespace qf {
namespace core {
namespace sql {
class DbFsDriver;
}
}
}

void qfsqldbfs_setdriver(qf::core::sql::DbFsDriver *drv);
//struct fuse_operations* qfsqldbfs_fuseops();

int qfsqldbfs_getattr(const char *path, struct stat *stbuf);
int qfsqldbfs_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi);
int qfsqldbfs_create(const char *path, mode_t mode, struct fuse_file_info *fi);
int qfsqldbfs_open(const char *path, struct fuse_file_info *fi);
int qfsqldbfs_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi);
int qfsqldbfs_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi);
int qfsqldbfs_fsync(const char *path, int isdatasync, struct fuse_file_info *fi);
int qfsqldbfs_flush(const char *path, struct fuse_file_info *fi);
int qfsqldbfs_release(const char *path, struct fuse_file_info *fi);
int qfsqldbfs_mknod(const char *path, mode_t mode, dev_t dev);
int qfsqldbfs_mkdir(const char *path, mode_t mode);
int qfsqldbfs_unlink(const char *path);
int qfsqldbfs_rmdir(const char *path);
int qfsqldbfs_utime(const char *path, struct utimbuf *ubuf);
int qfsqldbfs_truncate(const char *path, off_t new_size);
int qfsqldbfs_ftruncate(const char *path, off_t new_size, struct fuse_file_info *fi);
int qfsqldbfs_rename(const char *path, const char *new_path);
int qfsqldbfs_chmod(const char *path, mode_t mode);
int qfsqldbfs_chown(const char *path, uid_t uid, gid_t gid);

#endif // DBFSFUSEOPS_H
