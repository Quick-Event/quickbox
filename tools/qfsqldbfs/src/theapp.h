#ifndef THEAPP_H
#define THEAPP_H

#include <QCoreApplication>

class TheApp : public QCoreApplication
{
	Q_OBJECT
public:
	TheApp(int & argc, char **argv);
	~TheApp() Q_DECL_OVERRIDE {}

	Q_SLOT void onFuseThreadFinished();
};

#endif // THEAPP_H
