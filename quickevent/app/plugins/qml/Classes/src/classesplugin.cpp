#include "classesplugin.h"
#include "thispartwidget.h"
#include "classdocument.h"

#include <qf/qmlwidgets/framework/mainwindow.h>
#include <qf/qmlwidgets/action.h>

#include <qf/core/log.h>

#include <QQmlEngine>

namespace qfw = qf::qmlwidgets;
namespace qff = qf::qmlwidgets::framework;
//namespace qfd = qf::qmlwidgets::dialogs;
//namespace qfs = qf::core::sql;

ClassesPlugin::ClassesPlugin(QObject *parent)
	: Super(parent)
{
	connect(this, &ClassesPlugin::installed, this, &ClassesPlugin::onInstalled, Qt::QueuedConnection);
}

void ClassesPlugin::onInstalled()
{
	qff::MainWindow *fwk = qff::MainWindow::frameWork();
	m_partWidget = new ThisPartWidget();
	fwk->addPartWidget(m_partWidget, manifest()->featureId());

	emit nativeInstalled();
}

QObject *ClassesPlugin::createClassDocument(QObject *parent)
{
	ClassDocument *ret = new ClassDocument(parent);
	if(!parent) {
		qfWarning() << "Parent is NULL, created class will have QQmlEngine::JavaScriptOwnership.";
		qmlEngine()->setObjectOwnership(ret, QQmlEngine::JavaScriptOwnership);
	}
	return ret;
}

