#ifndef FUSETHREAD_H
#define FUSETHREAD_H

#include "thisfuse.h"

#include <QThread>

class FuseThread : public QThread
{
	Q_OBJECT
public:
	FuseThread(struct fuse *fuse_handle, struct fuse_chan *fuse_channel, const QString &mount_point, QObject *parent = nullptr);
	~FuseThread() Q_DECL_OVERRIDE {}

	void unmount();
protected:
	void run() Q_DECL_OVERRIDE;
private:
	struct fuse *m_fuseHandle;
	struct fuse_chan *m_fuseChannel;
	QString m_mountPoint;
};

#endif // FUSETHREAD_H
