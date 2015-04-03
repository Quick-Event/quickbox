#include "receipesplugin.h"
#include "../receipespartwidget.h"

#include <qf/qmlwidgets/framework/mainwindow.h>

namespace qff = qf::qmlwidgets::framework;

using namespace Receipes;

ReceipesPlugin::ReceipesPlugin(QObject *parent)
	: Super(parent)
{
	connect(this, &ReceipesPlugin::installed, this, &ReceipesPlugin::onInstalled);
}

void ReceipesPlugin::onInstalled()
{
	qff::MainWindow *framework = qff::MainWindow::frameWork();
	ReceipesPartWidget *pw = new ReceipesPartWidget(manifest()->featureId());
	framework->addPartWidget(pw);
}

QQmlListProperty<Receipes::CardChecker> ReceipesPlugin::cardCheckersListProperty()
{
	/// Generally this constructor should not be used in production code, as a writable QList violates QML's memory management rules.
	/// However, this constructor can be very useful while prototyping.
	return QQmlListProperty<Receipes::CardChecker>(this, m_cardCheckers);
}

