#include "cardreaderplugin.h"
#include "cardreaderpartwidget.h"

#include <qf/qmlwidgets/framework/mainwindow.h>

namespace qff = qf::qmlwidgets::framework;

CardReaderPlugin::CardReaderPlugin(QObject *parent)
	: Super(parent)
{
	connect(this, &CardReaderPlugin::installed, this, &CardReaderPlugin::onInstalled);
}

void CardReaderPlugin::onInstalled()
{
	qff::MainWindow *framework = qff::MainWindow::frameWork();
	CardReaderPartWidget *pw = new CardReaderPartWidget(manifest()->featureId());
	framework->addPartWidget(pw);
}

QQmlListProperty<CardReader::CardChecker> CardReaderPlugin::cardCheckersListProperty()
{
	/// Generally this constructor should not be used in production code, as a writable QList violates QML's memory management rules.
	/// However, this constructor can be very useful while prototyping.
	return QQmlListProperty<CardReader::CardChecker>(this, m_cardCheckers);
}

