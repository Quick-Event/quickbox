#include "competitorsplugin.h"
#include "../thispartwidget.h"
#include "competitordocument.h"
#include "../registrationswidget.h"

//#include <EventPlugin/eventplugin.h>

#include <qf/qmlwidgets/framework/mainwindow.h>
#include <qf/qmlwidgets/framework/dockwidget.h>
#include <qf/qmlwidgets/action.h>
#include <qf/qmlwidgets/menubar.h>

//#include <qf/core/log.h>

#include <QQmlEngine>

namespace qfw = qf::qmlwidgets;
namespace qff = qf::qmlwidgets::framework;
//namespace qfd = qf::qmlwidgets::dialogs;
//namespace qfs = qf::core::sql;

using namespace Competitors;

CompetitorsPlugin::CompetitorsPlugin(QObject *parent)
	: Super(parent)
{
	connect(this, &CompetitorsPlugin::installed, this, &CompetitorsPlugin::onInstalled, Qt::QueuedConnection);
}

CompetitorsPlugin::~CompetitorsPlugin()
{
	if(m_registrationsDockWidget)
		m_registrationsDockWidget->savePersistentSettingsRecursively();
}

QObject *CompetitorsPlugin::createCompetitorDocument(QObject *parent)
{
	CompetitorDocument *ret = new CompetitorDocument(parent);
	if(!parent) {
		qfWarning() << "Parent is NULL, created class will have QQmlEngine::JavaScriptOwnership.";
		qmlEngine()->setObjectOwnership(ret, QQmlEngine::JavaScriptOwnership);
	}
	return ret;
}

void CompetitorsPlugin::onInstalled()
{
	qff::MainWindow *fwk = qff::MainWindow::frameWork();
	m_partWidget = new ThisPartWidget();
	fwk->addPartWidget(m_partWidget, manifest()->featureId());
	{
		m_registrationsDockWidget = new qff::DockWidget(nullptr);
		m_registrationsDockWidget->setObjectName("registrationsDockWidget");
		m_registrationsDockWidget->setWindowTitle(tr("Registrations"));
		fwk->addDockWidget(Qt::RightDockWidgetArea, m_registrationsDockWidget);
		m_registrationsDockWidget->hide();
		connect(m_registrationsDockWidget, &qff::DockWidget::visibleChanged, this, &CompetitorsPlugin::onRegistrationsDockVisibleChanged);
	}
	{
		auto *a = m_registrationsDockWidget->toggleViewAction();
		//a->setCheckable(true);
		a->setShortcut(QKeySequence("ctrl+shift+R"));
		fwk->menuBar()->actionForPath("view")->addActionInto(a);
	}
	emit nativeInstalled();
}

void CompetitorsPlugin::onRegistrationsDockVisibleChanged(bool on)
{
	if(on && !m_registrationsDockWidget->widget()) {
		auto *rw = new RegistrationsWidget();
		qff::MainWindow *fwk = qff::MainWindow::frameWork();
		connect(fwk->plugin("Event"), SIGNAL(dbEventNotify(QString, QVariant)), rw, SLOT(onDbEvent(QString, QVariant)));
		m_registrationsDockWidget->setWidget(rw);
		rw->reload();
		//m_registrationsDockWidget->loadPersistentSettingsRecursively();
	}
}

