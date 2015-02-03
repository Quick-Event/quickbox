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

