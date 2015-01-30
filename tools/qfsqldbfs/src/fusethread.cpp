#include "fusethread.h"

#include <qf/core/log.h>

#include <signal.h>

FuseThread::FuseThread(struct fuse *fuse_handle, struct fuse_chan *fuse_channel, const QString &mount_point, QObject *parent)
	: QThread(parent), m_fuseHandle(fuse_handle), m_fuseChannel(fuse_channel), m_mountPoint(mount_point)
{

}

void FuseThread::unmount()
{
	// This will only cause fuse_loop_mt() to return in FuseThread::run()
	// if the mounpoint is not currently in use.
	fuse_unmount(m_mountPoint.toUtf8(), m_fuseChannel);
}

void FuseThread::run()
{
	qfLogFuncFrame();

	// Give FUSE the control. It will call functions in ops as they are requested by users of the FS.
	// Since fuse_loop_mt() is used instead of fuse_loop(), every call to the ops will be made in a new thread
	//fuse_loop_mt(m_fuseHandle);
	fuse_loop(m_fuseHandle);

	// FUSE has quit its event loop

	// Takes us to exitHandler()
	::raise(SIGQUIT);
}

