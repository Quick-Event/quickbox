#include "registrationswidget.h"
#include "ui_registrationswidget.h"
#include "Competitors/competitorsplugin.h"

#include <Event/eventplugin.h>

#include <qf/qmlwidgets/framework/mainwindow.h>

#include <qf/core/model/sqltablemodel.h>
#include <qf/core/sql/querybuilder.h>
#include <qf/core/sql/connection.h>
#include <qf/core/assert.h>

namespace qfm = qf::core::model;
namespace qfs = qf::core::sql;

static Competitors::CompetitorsPlugin* thisPlugin()
{
	qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
	auto plugin = qobject_cast<Competitors::CompetitorsPlugin *>(fwk->plugin("Competitors"));
	QF_ASSERT_EX(plugin != nullptr, "Bad plugin");
	return plugin;
}

static Event::EventPlugin* eventPlugin()
{
	qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
	auto *plugin = qobject_cast<Event::EventPlugin*>(fwk->plugin("Event"));
	QF_ASSERT_EX(plugin != nullptr, "Bad Event plugin!");
	return plugin;
}

RegistrationsWidget::RegistrationsWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::RegistrationsWidget)
{
	ui->setupUi(this);

	ui->tblRegistrationsTB->setTableView(ui->tblRegistrations);

	ui->tblRegistrations->setPersistentSettingsId("tblRegistrations");
	ui->tblRegistrations->setReadOnly(true);
	//ui->tblRegistrations->setInlineEditStrategy(qfw::TableView::OnCurrentFieldChange);
}

RegistrationsWidget::~RegistrationsWidget()
{
	delete ui;
}

void RegistrationsWidget::checkModel()
{
	qfLogFuncFrame();
	if(!isVisible())
		return;

	if(!ui->tblRegistrations->tableModel()) {
		qf::core::model::SqlTableModel *reg_model = thisPlugin()->registrationsModel();
		ui->tblRegistrations->setTableModel(reg_model);
		connect(reg_model, &qf::core::model::SqlTableModel::reloaded, [this]() {
			ui->tblRegistrations->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);
		});
	}
}

qf::qmlwidgets::TableView *RegistrationsWidget::tableView()
{
	return ui->tblRegistrations;
}

