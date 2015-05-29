#include "competitorwidget.h"
#include "ui_competitorwidget.h"

#include "competitordocument.h"
#include "registrationswidget.h"

#include <quickevent/og/itemdelegate.h>
#include <quickevent/og/sqltablemodel.h>
#include <quickevent/og/timems.h>

#include <qf/qmlwidgets/dialogs/dialog.h>

#include <qf/core/sql/dbenum.h>

namespace qfd = qf::qmlwidgets::dialogs;

CompetitorWidget::CompetitorWidget(QWidget *parent) :
	Super(parent),
	ui(new Ui::CompetitorWidget)
{
	setPersistentSettingsId("CompetitorWidget");
	ui->setupUi(this);

	setTitle(tr("competitor"));

	qf::qmlwidgets::ForeignKeyComboBox *cbx = ui->cbxClass;

	cbx->setReferencedTable("classes");
	cbx->setReferencedField("id");
	cbx->setReferencedCaptionField("name");

	dataController()->setDocument(new CompetitorDocument(this));
	m_runsModel = new quickevent::og::SqlTableModel(this);
	m_runsModel->addColumn("runs.offRace", tr("Off", "runs.offRace")).setToolTip(tr("Off race in this stage"));
	m_runsModel->addColumn("runs.stageId", tr("Stage")).setReadOnly(true);
	m_runsModel->addColumn("runs.startTimeMs", tr("Start")).setCastType(qMetaTypeId<quickevent::og::TimeMs>());
	m_runsModel->addColumn("runs.timeMs", tr("Time"))
			.setCastType(qMetaTypeId<quickevent::og::TimeMs>());
	m_runsModel->addColumn("runs.disqualified", tr("D", "runs.disqualified")).setToolTip(tr("Disqualified in this stage"));
	m_runsModel->addColumn("runs.cardError", tr("E", "runs.cardError")).setToolTip(tr("Card error in this stage"));
	m_runsModel->addColumn("runs.cardLent", tr("L", "runs.cardLent")).setToolTip(tr("Card lent in this stage"));
	quickevent::og::SqlTableModel::ColumnDefinition::DbEnumCastProperties runs_status_db_enum_cast_props;
	runs_status_db_enum_cast_props.setGroupName("runs.status");
	m_runsModel->addColumn("runs.status", tr("Status"))
			.setToolTip(tr("Run status in this stage"))
			.setCastType(qMetaTypeId<qf::core::sql::DbEnum>(), runs_status_db_enum_cast_props);
	ui->tblRuns->setTableModel(m_runsModel);
	ui->tblRuns->setPersistentSettingsId(ui->tblRuns->objectName());
	ui->tblRuns->setInlineEditSaveStrategy(qf::qmlwidgets::TableView::OnManualSubmit);
	ui->tblRuns->setItemDelegate(new quickevent::og::ItemDelegate(ui->tblRuns));

	connect(dataController()->document(), &qf::core::model::DataDocument::loaded, this, &CompetitorWidget::loadRunsTable);
	connect(dataController()->document(), &qf::core::model::DataDocument::saved, this, &CompetitorWidget::saveRunsTable);
}

CompetitorWidget::~CompetitorWidget()
{
	delete ui;
}

void CompetitorWidget::loadRunsTable()
{
	qf::core::model::DataDocument *doc = dataController()->document();
	qf::core::sql::QueryBuilder qb;
	qb.select2("runs", "*")
			.from("runs")
			.where("runs.competitorId=" QF_IARG(doc->value("competitors.id").toInt()))
			.orderBy("runs.stageId");
	m_runsModel->setQueryBuilder(qb);
	m_runsModel->reload();
}

void CompetitorWidget::saveRunsTable()
{
	qfLogFuncFrame();
	m_runsModel->postAll(false);
}

void CompetitorWidget::on_btChooseFromRegistrations_clicked()
{
	qfLogFuncFrame();
	auto *w = new RegistrationsWidget();
	w->setWindowTitle(tr("Find in registrations"));
	qfd::Dialog dlg(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
	//dlg.setDefaultButton(QDialogButtonBox::Ok);
	dlg.setCentralWidget(w);
	w->setFocusToWidget(RegistrationsWidget::FocusWidget::Registrations);
	if(dlg.exec()) {
		qf::core::utils::TableRow r = w->tableView()->tableRow();
		if(r.isNull())
			r = w->tableView()->tableRow(0);
		if(!r.isNull()) {
			qf::core::model::DataDocument*doc = dataController()->document();
			for(auto s : {"firstName", "lastName", "registration", "licence", "siId"}) {
				doc->setValue(s, r.value(s));
			}
		}
	}
}
