#ifndef THEAPP_H
#define THEAPP_H

#include <QCoreApplication>
#include <QSqlDriver>
#include <QVariant>

class TheApp : public QCoreApplication
{
	Q_OBJECT
public:
	TheApp(int & argc, char **argv);
	~TheApp() Q_DECL_OVERRIDE {}

	Q_SLOT void onFuseThreadFinished();
	//Q_SLOT void onSqlNotify(const QString &channel, QSqlDriver::NotificationSource source, const QVariant &payload);
};

#endif // THEAPP_H
