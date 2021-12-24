#include "relaywidget.h"
#include "ui_relaywidget.h"

#include "addlegdialogwidget.h"

#include "relaydocument.h"
#include "relaysplugin.h"

#include <plugins/Event/src/eventplugin.h>

#include <quickevent/gui/og/itemdelegate.h>
#include <quickevent/core/og/sqltablemodel.h>
#include <quickevent/core/og/timems.h>
#include <quickevent/core/si/siid.h>

#include <qf/qmlwidgets/dialogs/dialog.h>
#include <qf/qmlwidgets/dialogs/messagebox.h>
#include <qf/qmlwidgets/framework/mainwindow.h>
#include <qf/qmlwidgets/dialogbuttonbox.h>

#include <qf/core/sql/dbenum.h>
#include <qf/core/sql/transaction.h>
#include <qf/core/assert.h>
#include <qf/core/log.h>

#include <QMenu>
#include <QAction>
#include <QCompleter>
#include <QDate>
#include <QPushButton>

namespace qfd = qf::qmlwidgets::dialogs;
namespace qfw = qf::qmlwidgets;
namespace qfc = qf::core;
namespace qfs = qf::core::sql;

namespace {

class LegsModel : public quickevent::core::og::SqlTableModel
{
	Q_DECLARE_TR_FUNCTIONS(LegsModel)
private:
	using Super = quickevent::core::og::SqlTableModel;
public:
	LegsModel(QObject *parent = nullptr);

	enum Columns {
		col_runs_leg = 0,
		col_competitorName,
		col_runs_registration,
		col_runs_siId,
		col_runs_startTimeMs,
		col_runs_timeMs,
		col_runs_notCompeting,
		col_runs_misPunch,
		col_runs_disqualified,
		col_COUNT
	};

	QVariant value(int row_ix, int column_ix) const Q_DECL_OVERRIDE;
	bool setValue(int row_ix, int column_ix, const QVariant &val) Q_DECL_OVERRIDE;
};

LegsModel::LegsModel(QObject *parent)
	: Super(parent)
{
	clearColumns(col_COUNT);
	setColumn(col_runs_leg, ColumnDefinition("runs.leg", tr("Leg", "relays.leg")).setToolTip(tr("Leg")));
	setColumn(col_competitorName, ColumnDefinition("competitorName", tr("Name")).setReadOnly(true));
	setColumn(col_runs_registration, ColumnDefinition("competitors.registration", tr("Reg")));
	setColumn(col_runs_siId, ColumnDefinition("runs.siid", tr("SI")).setReadOnly(false).setCastType(qMetaTypeId<quickevent::core::si::SiId>()));
	setColumn(col_runs_startTimeMs, ColumnDefinition("runs.startTimeMs", tr("Start")).setCastType(qMetaTypeId<quickevent::core::og::TimeMs>()));
	setColumn(col_runs_timeMs, ColumnDefinition("runs.timeMs", tr("Time")).setCastType(qMetaTypeId<quickevent::core::og::TimeMs>()).setReadOnly(true));
	setColumn(col_runs_notCompeting, ColumnDefinition("runs.notCompeting", tr("NC", "runs.notCompeting")).setToolTip(tr("Not competing")));
	setColumn(col_runs_disqualified, ColumnDefinition("runs.disqualified", tr("D", "runs.disqualified")).setToolTip(tr("Disqualified")));
	setColumn(col_runs_misPunch, ColumnDefinition("runs.misPunch", tr("E", "runs.misPunch")).setToolTip(tr("Card mispunch")));
}

QVariant LegsModel::value(int row_ix, int column_ix) const
{
	return Super::value(row_ix, column_ix);
}

bool LegsModel::setValue(int row_ix, int column_ix, const QVariant &val)
{
	return Super::setValue(row_ix, column_ix, val);
}

}

RelayWidget:: RelayWidget(QWidget *parent) :
	Super(parent),
	ui(new Ui:: RelayWidget)
{
	qfLogFuncFrame();
	ui->setupUi(this);
	setPersistentSettingsId(objectName());

	setTitle(tr("Relay"));

	{
		qf::qmlwidgets::ForeignKeyComboBox *cbx = ui->cbxClass;
		cbx->setReferencedTable("classes");
		cbx->setReferencedField("id");
		cbx->setReferencedCaptionField("name");
	}

	dataController()->setDocument(new Relays:: RelayDocument(this));
	m_legsModel = new LegsModel(this);
	ui->tblLegs->setTableModel(m_legsModel);
	ui->tblLegs->setPersistentSettingsId(ui->tblLegs->objectName());
	ui->tblLegs->setDirtyRowsMenuSectionEnabled(false);
	ui->tblLegs->setInlineEditSaveStrategy(qf::qmlwidgets::TableView::OnEditedValueCommit);
	ui->tblLegs->setItemDelegate(new quickevent::gui::og::ItemDelegate(ui->tblLegs));
	//ui->tblLegs->setContextMenuPolicy(Qt::CustomContextMenu);
	//connect(ui->tblLegs, &qfw::TableView::customContextMenuRequested, this, & RelayWidget::onRunsTableCustomContextMenuRequest);

	connect(ui->btReloadLegsTable, &QPushButton::clicked, this, &RelayWidget::loadLegsTable);

	connect(ui->btAddLeg, &QPushButton::clicked, this, &RelayWidget::addLeg);
	connect(ui->btRemoveLeg, &QPushButton::clicked, this, &RelayWidget::removeLeg);
	connect(ui->btMoveLegUp, &QPushButton::clicked, this, &RelayWidget::moveLegUp);
	connect(ui->btMoveLegDown, &QPushButton::clicked, this, &RelayWidget::moveLegDown);
}

 RelayWidget::~ RelayWidget()
{
	delete ui;
}

bool  RelayWidget::loadLegsTable()
{
	qf::core::model::DataDocument *doc = dataController()->document();
	qf::core::sql::QueryBuilder qb;
	qb.select2("runs", "*")
			.select2("competitors", "registration")
			.select("COALESCE(competitors.lastName, '') || ' ' || COALESCE(competitors.firstName, '') AS competitorName")
			.from("runs")
			.join("runs.competitorId", "competitors.id")
			.where("runs.relayId=" + QString::number(doc->value("relays.id").toInt()))
			.where("runs.isRunning")
			.orderBy("runs.leg");
	m_legsModel->setQueryBuilder(qb, false);
	return m_legsModel->reload();
}

bool  RelayWidget::load(const QVariant &id, int mode)
{
	if(Super::load(id, mode))
		return loadLegsTable();
	return false;
}

bool  RelayWidget::saveData()
{
	Relays::RelayDocument*doc = qobject_cast<Relays:: RelayDocument*>(dataController()->document());
	if(doc->value(QStringLiteral("classId")).toInt() == 0) {
		qf::qmlwidgets::dialogs::MessageBox::showWarning(this, tr("Class should be entered."));
		return false;
	}
	if(doc->dataId() == 0) {
		qf::qmlwidgets::dialogs::MessageBox::showWarning(this, tr("Relay ID invalid."));
		return false;
	}
	bool ret = false;
	try {
		ret = Super::saveData();
		if(ret)
			checkLegsStartTimes();
	}
	catch (qf::core::Exception &e) {
		qf::qmlwidgets::dialogs::MessageBox::showException(this, e);
	}
	return ret;
}

void RelayWidget::checkLegsStartTimes()
{
	for (int i = 0; i < m_legsModel->rowCount(); ++i) {
		int leg = m_legsModel->value(i, LegsModel::col_runs_leg).toInt();
		if(leg == 1 && m_legsModel->value(i, LegsModel::col_runs_startTimeMs).isNull()) {
			/// assign class start time
			int run_id = m_legsModel->tableRow(i).value(QStringLiteral("runs.id")).toInt();
			qf::core::model::DataDocument *doc = dataDocument();
			int class_id = doc->value(QStringLiteral("relays.classId")).toInt();
			qf::core::sql::Query q;
			q.execThrow("SELECT startTimeMin FROM classdefs WHERE classId=" QF_IARG(class_id));
			if(q.next()) {
				int start_time = q.value(0).toInt() * 60 * 1000;
				q.execThrow("UPDATE runs SET startTimeMs=" QF_IARG(start_time) " WHERE id=" QF_IARG(run_id));
			}
			break;
		}
	}
}

void RelayWidget::addLeg()
{
	if(!saveData())
		return;
	Relays::RelayDocument*doc = qobject_cast<Relays:: RelayDocument*>(dataController()->document());
	auto *w = new AddLegDialogWidget();
	w->setClassId(doc->value(QStringLiteral("classId")).toInt());
	w->setRelayId(doc->dataId().toInt());
	w->setWindowTitle(tr("Add leg"));
	connect(w, &AddLegDialogWidget::legAdded, this, &RelayWidget::loadLegsTable);
	qfd::Dialog dlg(this);
	/*
	//dlg.setDefaultButton(QDialogButtonBox::Ok);
	QPushButton *bt_add_current = dlg.buttonBox()->addButton(tr("Add current"), QDialogButtonBox::AcceptRole);
	bool add_current = false;
	connect(dlg.buttonBox(), &qf::qmlwidgets::DialogButtonBox::clicked, [&add_current, bt_add_current](QAbstractButton *button) {
		add_current = (button == bt_add_current);
	});
	*/
	dlg.setCentralWidget(w);
	dlg.exec();
}

void RelayWidget::removeLeg()
{
	qfLogFuncFrame();
	qf::core::utils::TableRow row = ui->tblLegs->selectedRow();
	int run_id = row.value("runs.id").toInt();
	qfDebug() << "run id:" << run_id;
	if(run_id > 0) {
		qf::core::sql::Query q;
		q.exec("DELETE FROM runs WHERE id=" + QString::number(run_id), qf::core::Exception::Throw);
		loadLegsTable();
	}
}

void RelayWidget::moveLegUp()
{
	QModelIndex curr_ix = ui->tblLegs->currentIndex();
	if(!curr_ix.isValid())
		return;
	qf::core::utils::TableRow row = ui->tblLegs->selectedRow();
	int leg = row.value("runs.leg").toInt();
	if(leg <= 1)
		return;
	Relays::RelayDocument*doc = qobject_cast<Relays:: RelayDocument*>(dataController()->document());
	int relay_id = doc->dataId().toInt();
	int run_id = row.value("runs.id").toInt();
	QVariant run_stime = row.value("startTimeMs");
	int run_prev_id = 0;
	QVariant run_prev_stime;
	qf::core::sql::Query q;
	q.exec("SELECT id, startTimeMs FROM runs WHERE"
		   " relayId=" + QString::number(relay_id)
		   + " AND leg=" + QString::number(leg - 1)
		   , qf::core::Exception::Throw);
	if(q.next()) {
		run_prev_id = q.value(0).toInt();
		run_prev_stime = q.value(1);
	}
	if(run_prev_id > 0) {
		q.exec("UPDATE runs SET"
			    " leg=" + QString::number(leg)
			   + ", startTimeMs=" + (run_stime.isNull()? QStringLiteral("NULL"): run_stime.toString())
			   + " WHERE "
			   + " id=" + QString::number(run_prev_id)
			   , qf::core::Exception::Throw);
	}
	q.exec("UPDATE runs SET"
		    " leg=" + QString::number(leg - 1)
		   + ", startTimeMs=" + (run_prev_stime.isNull()? QStringLiteral("NULL"): run_prev_stime.toString())
		   + " WHERE "
		   + " id=" + QString::number(run_id)
		   , qf::core::Exception::Throw);
	loadLegsTable();
	if(curr_ix.row() > 0) {
		curr_ix = ui->tblLegs->model()->index(curr_ix.row()-1, curr_ix.column());
		ui->tblLegs->setCurrentIndex(curr_ix);
	}
}

void RelayWidget::moveLegDown()
{
	QModelIndex curr_ix = ui->tblLegs->currentIndex();
	if(!curr_ix.isValid())
		return;
	qf::core::model::TableModel *m = ui->tblLegs->tableModel();
	int max_leg = 0;
	for (int i = 0; i < m->rowCount(); ++i) {
		int l = m->value(i, "runs.leg").toInt();
		if(l > max_leg)
			max_leg = l;
	}
	qf::core::utils::TableRow row = ui->tblLegs->selectedRow();
	int leg = row.value("runs.leg").toInt();
	if(leg >= max_leg)
		return;
	Relays::RelayDocument*doc = qobject_cast<Relays:: RelayDocument*>(dataController()->document());
	int relay_id = doc->dataId().toInt();
	int run_id = row.value("runs.id").toInt();
	QVariant run_stime = row.value("startTimeMs");
	int run_next_id = 0;
	QVariant run_next_stime;
	qf::core::sql::Query q;
	q.exec("SELECT id FROM runs WHERE"
		   " relayId=" + QString::number(relay_id)
		   + " AND leg=" + QString::number(leg + 1)
		   , qf::core::Exception::Throw);
	if(q.next()) {
		run_next_id = q.value(0).toInt();
		run_next_stime = q.value(1);
	}
	if(run_next_id > 0) {
		q.exec("UPDATE runs SET"
			    " leg=" + QString::number(leg)
			   + ", startTimeMs=" + (run_stime.isValid()? run_stime.toString(): QStringLiteral("NULL"))
			   + " WHERE "
			   + " id=" + QString::number(run_next_id)
			   , qf::core::Exception::Throw);
	}
	q.exec("UPDATE runs SET"
		    " leg=" + QString::number(leg + 1)
		   + ", startTimeMs=" + (run_next_stime.isValid()? run_next_stime.toString(): QStringLiteral("NULL"))
		   + " WHERE "
		   + " id=" + QString::number(run_id)
		   , qf::core::Exception::Throw);
	loadLegsTable();
	if(curr_ix.row() < m->rowCount() - 1) {
		curr_ix = ui->tblLegs->model()->index(curr_ix.row()+1, curr_ix.column());
		ui->tblLegs->setCurrentIndex(curr_ix);
	}
}

