#include "cardreadoutplugin.h"
#include "cardreadoutpartwidget.h"

#include <qf/qmlwidgets/framework/mainwindow.h>

namespace qff = qf::qmlwidgets::framework;

CardReadoutPlugin::CardReadoutPlugin(QObject *parent)
	: Super(parent)
{
	connect(this, &CardReadoutPlugin::installed, this, &CardReadoutPlugin::onInstalled, Qt::QueuedConnection);
}

void CardReadoutPlugin::onInstalled()
{
	qff::MainWindow *framework = qff::MainWindow::frameWork();
	CardReadoutPartWidget *pw = new CardReadoutPartWidget();
	framework->addPartWidget(pw, manifest()->featureId());
}

