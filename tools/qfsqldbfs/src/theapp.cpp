#include "theapp.h"

#include <qf/core/log.h>

TheApp::TheApp(int &argc, char **argv)
	: QCoreApplication(argc, argv)
{
}

void TheApp::onFuseThreadFinished()
{
	qfInfo() << "calling core application quit";
	quit();
}

void TheApp::onSqlNotify(const QString &channel, QSqlDriver::NotificationSource source, const QVariant &payload)
{
	qfInfo() << "APP GOT NOTIFY" << source << channel << payload;
}

void TheApp::onSqlNotify2(const QString &channel)
{
	qfInfo() << "APP GOT NOTIFY2" << channel;
}

