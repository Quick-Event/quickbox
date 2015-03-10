#include "coreplugin.h"
#include "widgets/appstatusbar.h"

#include <qf/qmlwidgets/framework/mainwindow.h>

namespace qff = qf::qmlwidgets::framework;

CorePlugin::CorePlugin(QObject *parent)
	: Super(parent)
{
	connect(this, &CorePlugin::installed, this, &CorePlugin::onInstalled);//, Qt::QueuedConnection);
}

void CorePlugin::onInstalled()
{
	qff::MainWindow *fwk = qff::MainWindow::frameWork();
	fwk->setStatusBar(new AppStatusBar());
}

