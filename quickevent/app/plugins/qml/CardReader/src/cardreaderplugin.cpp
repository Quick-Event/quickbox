#include "cardreaderplugin.h"
#include "cardreaderpartwidget.h"

#include <qf/qmlwidgets/framework/mainwindow.h>

namespace qff = qf::qmlwidgets::framework;

CardReaderPlugin::CardReaderPlugin(QObject *parent)
	: Super(parent)
{
	connect(this, &CardReaderPlugin::installed, this, &CardReaderPlugin::onInstalled, Qt::QueuedConnection);
}

void CardReaderPlugin::onInstalled()
{
	qff::MainWindow *framework = qff::MainWindow::frameWork();
	CardReaderPartWidget *pw = new CardReaderPartWidget();
	framework->addPartWidget(pw, manifest()->featureId());
}

