#include "registrationswidget.h"
#include "ui_registrationswidget.h"
#include "competitorsplugin.h"

#include <Event/eventplugin.h>

#include <qf/qmlwidgets/framework/mainwindow.h>

#include <qf/core/model/sqltablemodel.h>
#include <qf/core/sql/querybuilder.h>
#include <qf/core/sql/connection.h>
#include <qf/core/assert.h>

namespace qfm = qf::core::model;
namespace qfs = qf::core::sql;
using qf::qmlwidgets::framework::getPlugin;
using Competitors::CompetitorsPlugin;

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
		qf::core::model::SqlTableModel *reg_model = getPlugin<CompetitorsPlugin>()->registrationsModel();
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

