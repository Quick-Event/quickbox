#include "competitorsplugin.h"
#include "competitordocument.h"
#include "plugins/Core/src/widgets/settingsdialog.h"
#include "registrationswidget.h"
#include "competitorwidget.h"
#include "competitorswidget.h"
#include "lentcardssettingspage.h"

#include <qf/qmlwidgets/framework/application.h>
#include <qf/qmlwidgets/framework/mainwindow.h>
#include <qf/qmlwidgets/framework/dockwidget.h>
#include <qf/qmlwidgets/dialogs/dialog.h>
#include <qf/qmlwidgets/action.h>
#include <qf/qmlwidgets/menubar.h>

#include <qf/core/model/sqltablemodel.h>
#include <qf/core/log.h>
#include <qf/core/assert.h>
#include <plugins/Event/src/eventplugin.h>
#include <plugins/Core/src/coreplugin.h>

#include <QQmlEngine>

namespace qfw = qf::qmlwidgets;
namespace qff = qf::qmlwidgets::framework;
namespace qfd = qf::qmlwidgets::dialogs;
namespace qfm = qf::core::model;
namespace qfs = qf::core::sql;
using ::PartWidget;
using qff::getPlugin;
using Event::EventPlugin;

namespace Competitors {

CompetitorsPlugin::CompetitorsPlugin(QObject *parent)
	: Super("Competitors", parent)
{
	connect(this, &CompetitorsPlugin::installed, this, &CompetitorsPlugin::onInstalled);
}

CompetitorsPlugin::~CompetitorsPlugin()
{
	//if(m_registrationsDockWidget)
	//	m_registrationsDockWidget->savePersistentSettingsRecursively();
}

QObject *CompetitorsPlugin::createCompetitorDocument(QObject *parent)
{
	CompetitorDocument *ret = new CompetitorDocument(parent);
	return ret;
}

int CompetitorsPlugin::editCompetitor(int id, int mode)
{
	qfLogFuncFrame() << "id:" << id;
	auto *w = new CompetitorWidget();
	w->setWindowTitle(tr("Edit Competitor"));
	qfd::Dialog dlg(QDialogButtonBox::Save | QDialogButtonBox::Cancel, m_partWidget);
	dlg.setDefaultButton(QDialogButtonBox::Save);
	dlg.setCentralWidget(w);
	w->load(id, (qfm::DataDocument::RecordEditMode)mode);
	return dlg.exec();
}

void CompetitorsPlugin::onInstalled()
{
	qff::MainWindow *fwk = qff::MainWindow::frameWork();
	m_partWidget = qff::initPluginWidget<CompetitorsWidget, PartWidget>(tr("&Competitors"), featureId());
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

	connect(getPlugin<EventPlugin>(), &Event::EventPlugin::eventOpenChanged, this, &CompetitorsPlugin::reloadRegistrationsModel);
	connect(getPlugin<EventPlugin>(), &Event::EventPlugin::dbEventNotify, this, &CompetitorsPlugin::onDbEventNotify);


	auto core_plugin = qf::qmlwidgets::framework::getPlugin<Core::CorePlugin>();
	core_plugin->settingsDialog()->addPage(new LentCardsSettingsPage());

	emit nativeInstalled();
}

void CompetitorsPlugin::onRegistrationsDockVisibleChanged(bool on)
{
	if(on && !m_registrationsDockWidget->widget()) {
		auto *rw = new RegistrationsWidget();
		m_registrationsDockWidget->setWidget(rw);
		rw->checkModel();
	}
}

void CompetitorsPlugin::onDbEventNotify(const QString &domain, int connection_id, const QVariant &data)
{
	Q_UNUSED(connection_id)
	qfLogFuncFrame() << "domain:" << domain << "payload:" << data;
	if(domain == QLatin1String(Event::EventPlugin::DBEVENT_REGISTRATIONS_IMPORTED))
		reloadRegistrationsModel();
	emit dbEventNotify(domain, connection_id, data);
}

void CompetitorsPlugin::reloadRegistrationsModel()
{
	qfLogFuncFrame() << "isEventOpen():" << getPlugin<EventPlugin>()->isEventOpen();
	if(getPlugin<EventPlugin>()->isEventOpen())
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
		m_registrationsModel->setQueryBuilder(qb, false);
	}
	return m_registrationsModel;
}

const qf::core::utils::Table &CompetitorsPlugin::registrationsTable()
{
	qf::core::model::SqlTableModel *m = registrationsModel();
	if(m_registrationsTable.isNull() && !m->table().isNull()) {
		m_registrationsTable = m->table();
		auto c_nsk = QStringLiteral("competitorNameAscii7");
		m_registrationsTable.appendColumn(c_nsk, QMetaType::QString);
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
