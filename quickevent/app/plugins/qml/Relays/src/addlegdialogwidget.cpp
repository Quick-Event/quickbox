#include "addlegdialogwidget.h"
#include "ui_addlegdialogwidget.h"

#include <Competitors/competitordocument.h>
#include <Competitors/competitorsplugin.h>

#include <qf/qmlwidgets/framework/mainwindow.h>

#include <qf/core/log.h>
#include <qf/core/exception.h>
#include <qf/core/assert.h>
#include <qf/core/model/sqltablemodel.h>

static Competitors::CompetitorsPlugin* competitorsPlugin()
{
	qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
	auto *plugin = qobject_cast<Competitors::CompetitorsPlugin*>(fwk->plugin("Competitors"));
	QF_ASSERT_EX(plugin != nullptr, "Bad Competitors plugin!");
	return plugin;
}

AddLegDialogWidget::AddLegDialogWidget(QWidget *parent)
	: Super(parent)
	, ui(new Ui::AddLegDialogWidget)
{
	qfLogFuncFrame() << objectName();
	ui->setupUi(this);
	setPersistentSettingsId(objectName());
	ui->tblCompetitors->setPersistentSettingsId(ui->tblCompetitors->objectName());
	ui->tblRegistrations->setPersistentSettingsId(ui->tblRegistrations->objectName());

	qf::core::model::SqlTableModel *competitors_model = new qf::core::model::SqlTableModel(this);
	qf::core::sql::QueryBuilder qb;
	qb.select2("runs", "leg")
			.select2("competitors", "id, registration, siId")
			.select2("classes", "name")
			.select("COALESCE(lastName, '') || ' ' || COALESCE(firstName, '') AS competitorName")
			.from("runs")
			.join("runs.competitorId", "competitors.id")
			.join("runs.relayId", "relays.id")
			.join("competitors.classId", "classes.id")
			.where("runs.isRunning")
			.orderBy("competitorName");//.limit(10);
	competitors_model->setQueryBuilder(qb);
	ui->tblCompetitors->setTableModel(competitors_model);
	ui->tblCompetitors->setReadOnly(true);
	competitors_model->reload();

	qf::core::model::SqlTableModel *reg_model = competitorsPlugin()->registrationsModel();
	ui->tblRegistrations->setTableModel(reg_model);
	ui->tblRegistrations->setReadOnly(true);
	connect(reg_model, &qf::core::model::SqlTableModel::reloaded, [this]() {
		ui->tblRegistrations->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);
	});

	connect(ui->edFilter, &QLineEdit::textChanged, this, &AddLegDialogWidget::onFilterTextChanged);
	connect(ui->tblRegistrations, &qf::qmlwidgets::TableView::doubleClicked, this, &AddLegDialogWidget::onRegistrationSelected);
}

AddLegDialogWidget::~AddLegDialogWidget()
{
	delete ui;
}

void AddLegDialogWidget::onFilterTextChanged()
{
	QString txt = ui->edFilter->text().trimmed();
	if(txt.length() < 3)
		return;
	ui->tblCompetitors->filterByString(txt);
	ui->tblRegistrations->filterByString(txt);
	ui->edFilter->setFocus();
}

void AddLegDialogWidget::onRegistrationSelected()
{
	qf::core::utils::TableRow row = ui->tblRegistrations->selectedRow();
	Competitors::CompetitorDocument doc;
	doc.loadForInsert();
	doc.setValue("firstName", row.value("firstName"));
	doc.setValue("lastName", row.value("lastName"));
	doc.setValue("registration", row.value("registration"));
	doc.setValue("licence", row.value("licence"));
	doc.setValue("siid", row.value("siid"));
	doc.setValue("classId", classId());
	doc.save();
	int run_id = doc.lastInsertedRunsIds().value(0);
	QF_ASSERT(run_id > 0, "Bad insert", return);
	int max_leg = 0;
	qf::core::sql::Query q;
	q.exec("SELECT MAX(leg) FROM runs WHERE leg IS NOT NULL AND relayId=" + QString::number(relayId()), qf::core::Exception::Throw);
	if(q.next())
		max_leg = q.value(0).toInt();
	q.exec("UPDATE runs SET relayId=" + QString::number(relayId()) + ", leg=" + QString::number(max_leg+1)
		   + " WHERE id=" + QString::number(run_id), qf::core::Exception::Throw);
	emit legAdded();
}
