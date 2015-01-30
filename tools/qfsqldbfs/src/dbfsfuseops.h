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
int qfsqldbfs_getattr(const char *path, struct stat *stbuf);
int qfsqldbfs_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi);
int qfsqldbfs_open(const char *path, struct fuse_file_info *fi);
int qfsqldbfs_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi);
int qfsqldbfs_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi);
int qfsqldbfs_flush(const char *path, struct fuse_file_info *fi);
int qfsqldbfs_mknod(const char *path, mode_t mode, dev_t dev);
int qfsqldbfs_mkdir(const char *path, mode_t mode);
int qfsqldbfs_unlink(const char *path);
int qfsqldbfs_rmdir(const char *path);

#endif // DBFSFUSEOPS_H
