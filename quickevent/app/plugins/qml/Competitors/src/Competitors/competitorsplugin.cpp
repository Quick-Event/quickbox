#include "competitorsplugin.h"
#include "../thispartwidget.h"
#include "competitordocument.h"
#include "../registrationswidget.h"

#include <Event/eventplugin.h>

#include <qf/qmlwidgets/framework/mainwindow.h>
#include <qf/qmlwidgets/framework/dockwidget.h>
#include <qf/qmlwidgets/action.h>
#include <qf/qmlwidgets/menubar.h>

#include <qf/core/model/sqltablemodel.h>
#include <qf/core/log.h>
#include <qf/core/assert.h>

#include <QQmlEngine>

namespace qfw = qf::qmlwidgets;
namespace qff = qf::qmlwidgets::framework;
namespace qfm = qf::core::model;
namespace qfs = qf::core::sql;

namespace Competitors {

static Event::EventPlugin* eventPlugin()
{
	qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
	auto *plugin = qobject_cast<Event::EventPlugin*>(fwk->plugin("Event"));
	QF_ASSERT_EX(plugin != nullptr, "Bad Event plugin!");
	return plugin;
}

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
		connect(m_registrationsDockWidget, &qff::DockWidget::visibilityChanged, this, &CompetitorsPlugin::onRegistrationsDockVisibleChanged);

		auto *a = m_registrationsDockWidget->toggleViewAction();
		//a->setCheckable(true);
		a->setShortcut(QKeySequence("ctrl+shift+R"));
		fwk->menuBar()->actionForPath("view")->addActionInto(a);
	}

	connect(eventPlugin(), &Event::EventPlugin::eventOpenChanged, this, &CompetitorsPlugin::reloadRegistrationsModel);
	connect(eventPlugin(), &Event::EventPlugin::dbEventNotify, this, &CompetitorsPlugin::onDbEventNotify);

	emit nativeInstalled();
}

void CompetitorsPlugin::onRegistrationsDockVisibleChanged(bool on)
{
	if(on && !m_registrationsDockWidget->widget()) {
		auto *rw = new RegistrationsWidget();
		m_registrationsDockWidget->setWidget(rw);
		rw->checkModel();
		//if(eventPlugin()->isDbOpen())
		//	rw->reload();
		//m_registrationsDockWidget->loadPersistentSettingsRecursively();
	}
}

void CompetitorsPlugin::onDbEventNotify(const QString &domain, const QVariant &payload)
{
	qfLogFuncFrame() << "domain:" << domain << "payload:" << payload;
	if(domain == QLatin1String(Event::EventPlugin::DBEVENT_REGISTRATIONS_IMPORTED))
		reloadRegistrationsModel();
	emit dbEventNotify(domain, payload);
}

void CompetitorsPlugin::reloadRegistrationsModel()
{
	qfLogFuncFrame() << "isEventOpen():" << eventPlugin()->isEventOpen();
	if(eventPlugin()->isEventOpen())
		registrationsModel()->reload();
	else
		registrationsModel()->clearRows();
	// clear registration table to be regenerated when registrationsTable() will be called
	m_registrationsTable = qf::core::utils::Table();
}

qf::core::model::SqlTableModel* CompetitorsPlugin::registrationsModel()
{
	if(!m_registrationsModel) {
		m_registrationsModel = new qf::core::model::SqlTableModel(this);
		m_registrationsModel->addColumn("competitorName", tr("Name"));
		m_registrationsModel->addColumn("registration", tr("Reg"));
		m_registrationsModel->addColumn("licence", tr("Lic"));
		m_registrationsModel->addColumn("siId", tr("SI"));
		//m_registrationsModel->addColumn("fistName");
		//m_registrationsModel->addColumn("lastName");
		qfs::QueryBuilder qb;
		qb.select2("registrations", "firstName, lastName, licence, registration, siId")
				.select("COALESCE(lastName, '') || ' ' || COALESCE(firstName, '') AS competitorName")
				.from("registrations")
				.orderBy("lastName, firstName");
		m_registrationsModel->setQueryBuilder(qb);
	}
	return m_registrationsModel;
}

const qf::core::utils::Table &CompetitorsPlugin::registrationsTable()
{
	qf::core::model::SqlTableModel *m = registrationsModel();
	if(m_registrationsTable.isNull() && !m->table().isNull()) {
		m_registrationsTable = m->table();
		auto c_nsk = QStringLiteral("competitorNameAscii7");
		m_registrationsTable.appendColumn(c_nsk, QVariant::String);
		int ix_nsk = m_registrationsTable.fields().fieldIndex(c_nsk);
		int ix_cname = m_registrationsTable.fields().fieldIndex(QStringLiteral("competitorName"));
		for (int i = 0; i < m_registrationsTable.rowCount(); ++i) {
			qf::core::utils::TableRow &row_ref = m_registrationsTable.rowRef(i);
			QString nsk = row_ref.value(ix_cname).toString();
			nsk = QString::fromLatin1(qf::core::Collator::toAscii7(QLocale::Czech, nsk, true));
			row_ref.setValue(ix_nsk, nsk);
		}
	}
	return m_registrationsTable;
}

}
