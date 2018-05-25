#include "relaywidget.h"
#include "ui_relaywidget.h"

#include "addlegdialogwidget.h"

#include "Relays/relaydocument.h"
#include "Relays/relaysplugin.h"

#include "Event/eventplugin.h"

#include <quickevent/og/itemdelegate.h>
#include <quickevent/og/sqltablemodel.h>
#include <quickevent/og/timems.h>
#include <quickevent/si/siid.h>

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

class BadDataInputException : public std::runtime_error
{
public:
	BadDataInputException(const QString &message) : std::runtime_error(""), m_message(message) {}
	~BadDataInputException() Q_DECL_OVERRIDE {}

	const QString& message() const {return m_message;}
private:
	QString m_message;
};

class LegsModel : public quickevent::og::SqlTableModel
{
	using Super = quickevent::og::SqlTableModel;
public:
	LegsModel(QObject *parent = nullptr);

	enum Columns {
		col_runs_leg = 0,
		col_competitorName,
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
	setColumn(col_runs_siId, ColumnDefinition("runs.siid", tr("SI")).setReadOnly(false).setCastType(qMetaTypeId<quickevent::si::SiId>()));
	setColumn(col_runs_startTimeMs, ColumnDefinition("runs.startTimeMs", tr("Start")).setCastType(qMetaTypeId<quickevent::og::TimeMs>()).setReadOnly(true));
	setColumn(col_runs_timeMs, ColumnDefinition("runs.timeMs", tr("Time")).setCastType(qMetaTypeId<quickevent::og::TimeMs>()).setReadOnly(true));
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
/*
static Relays::RelaysPlugin* competitorsPlugin()
{
	qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
	qf::qmlwidgets::framework::Plugin *plugin = fwk->plugin("Relays");
	return qobject_cast<Relays::RelaysPlugin *>(plugin);
}

static Event::EventPlugin* eventPlugin()
{
	qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
	auto *plugin = qobject_cast<Event::EventPlugin*>(fwk->plugin("Event"));
	QF_ASSERT_EX(plugin != nullptr, "Bad event plugin!");
	return plugin;
}
*/

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
	ui->tblLegs->setInlineEditSaveStrategy(qf::qmlwidgets::TableView::OnEditedValueCommit);
	ui->tblLegs->setItemDelegate(new quickevent::og::ItemDelegate(ui->tblLegs));
	//ui->tblLegs->setContextMenuPolicy(Qt::CustomContextMenu);
	//connect(ui->tblLegs, &qfw::TableView::customContextMenuRequested, this, & RelayWidget::onRunsTableCustomContextMenuRequest);
	connect(ui->btAddLeg, &QPushButton::clicked, this, &RelayWidget::addLeg);
	connect(ui->btRemoveLeg, &QPushButton::clicked, this, &RelayWidget::removeLeg);
	connect(ui->btMoveLegUp, &QPushButton::clicked, this, &RelayWidget::moveLegUp);
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
			.select("COALESCE(competitors.lastName, '') || ' ' || COALESCE(competitors.firstName, '') AS competitorName")
			.from("runs")
			.join("runs.competitorId", "competitors.id")
			.where("runs.relayId=" QF_IARG(doc->value("relays.id").toInt()))
			.where("runs.isRunning")
			.orderBy("runs.leg");
	m_legsModel->setQueryBuilder(qb, false);
	return m_legsModel->reload();
}
/*
bool  RelayWidget::saveLegsTable()
{
	qfLogFuncFrame();
	bool ret = m_legsModel->postAll(true);
	return ret;
}
*/
/*
void  RelayWidget::onRunsTableCustomContextMenuRequest(const QPoint &pos)
{
	qfLogFuncFrame();
	QAction a_show_in_runs(tr("Show in runs table"), nullptr);
	QList<QAction*> lst;
	lst << &a_show_in_runs;
	QAction *a = QMenu::exec(lst, ui->tblLegs->viewport()->mapToGlobal(pos));
	if(a == &a_show_in_runs) {
		auto row = ui->tblLegs->tableRow();
		int stage_no = row.value("stageId").toInt();
		int class_id = row.value("classId").toInt();
		int competitor_id = row.value("competitorId").toInt();
		//QMetaObject::invokeMethod(this, "accept", Qt::QueuedConnection);
		emit editStartListRequest(stage_no, class_id, competitor_id);
	}
}
*/
bool  RelayWidget::load(const QVariant &id, int mode)
{
	if(Super::load(id, mode))
		return loadLegsTable();
	return false;
}
/*
void  RelayWidget::showRunsTable(int stage_id)
{
	if(!saveData())
		return;
	qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
	QObject *runs_plugin = fwk->plugin("Runs");
	if(runs_plugin) {
		qf::core::model::DataDocument*doc = dataController()->document();
		int competitor_id = doc->value("competitors.id").toInt();
		int class_id = ui->cbxClass->currentData().toInt();
		QString sort_col = QStringLiteral("runs.startTimeMs");
		QMetaObject::invokeMethod(runs_plugin, "showRunsTable"
								  , Q_ARG(int, stage_id)
								  , Q_ARG(int, class_id)
								  , Q_ARG(bool, false)
								  , Q_ARG(QString, sort_col)
								  , Q_ARG(int, competitor_id));
		loadLegsTable();
	}
}
*/

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
		/*
		qf::core::sql::Transaction transaction;
		if(Super::saveData())
			ret = saveLegsTable();
		transaction.commit();
		*/
	}
	catch (qf::core::Exception &e) {
		qf::qmlwidgets::dialogs::MessageBox::showException(this, e);
	}
	return ret;
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
	int run_prev_id = 0;
	qf::core::sql::Query q;
	q.exec("SELECT id FROM runs WHERE"
		   " relayId=" + QString::number(relay_id)
		   + " AND leg=" + QString::number(leg - 1)
		   , qf::core::Exception::Throw);
	if(q.next())
		run_prev_id = q.value(0).toInt();
	if(run_prev_id > 0) {
		q.exec("UPDATE runs SET leg=" + QString::number(leg)
			   + " WHERE "
			   + " id=" + QString::number(run_prev_id)
			   , qf::core::Exception::Throw);
	}
	q.exec("UPDATE runs SET leg=" + QString::number(leg - 1)
		   + " WHERE "
		   + " id=" + QString::number(run_id)
		   , qf::core::Exception::Throw);
	loadLegsTable();
	if(curr_ix.row() > 0) {
		curr_ix = ui->tblLegs->model()->index(curr_ix.row()-1, curr_ix.column());
		ui->tblLegs->setCurrentIndex(curr_ix);
	}
}

