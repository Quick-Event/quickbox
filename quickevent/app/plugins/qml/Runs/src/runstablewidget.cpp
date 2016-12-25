#include "runstablewidget.h"
#include "ui_runstablewidget.h"
#include "runstablemodel.h"
#include "runstableitemdelegate.h"
#include "Runs/runsplugin.h"

#include <Event/eventplugin.h>

#include <quickevent/si/siid.h>
#include <quickevent/og/timems.h>

#include <qf/qmlwidgets/dialogs/messagebox.h>
#include <qf/qmlwidgets/framework/mainwindow.h>
#include <qf/qmlwidgets/framework/plugin.h>

#include <qf/core/sql/transaction.h>
#include <qf/core/log.h>
#include <qf/core/assert.h>

#include <QSortFilterProxyModel>
#include <QMenu>
#include <QTimer>
#include <QInputDialog>

namespace qfs = qf::core::sql;
namespace qfw = qf::qmlwidgets;
namespace qff = qf::qmlwidgets::framework;
namespace qfd = qf::qmlwidgets::dialogs;
namespace qfm = qf::core::model;

static Runs::RunsPlugin *runsPlugin()
{
	qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
	auto *plugin = qobject_cast<Runs::RunsPlugin *>(fwk->plugin("Runs"));
	QF_ASSERT(plugin != nullptr, "Runs plugin not installed!", return nullptr);
	return plugin;
}
/*
static Event::EventPlugin *eventPlugin()
{
	qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
	auto *plugin = qobject_cast<Event::EventPlugin *>(fwk->plugin("Event"));
	QF_ASSERT(plugin != nullptr, "Event plugin not installed!", return nullptr);
	return plugin;
}
*/
RunsTableWidget::RunsTableWidget(QWidget *parent) :
	Super(parent),
	ui(new Ui::RunsTableWidget)
{
	ui->setupUi(this);

	ui->tblRunsToolBar->setTableView(ui->tblRuns);

	ui->tblRuns->setShowExceptionDialog(false);
	connect(ui->tblRuns, &qf::qmlwidgets::TableView::sqlException, this, &RunsTableWidget::onTableViewSqlException, Qt::QueuedConnection);
	ui->tblRuns->setInsertRowEnabled(false);
	ui->tblRuns->setRemoveRowEnabled(false);
	ui->tblRuns->setCloneRowEnabled(false);
	ui->tblRuns->setPersistentSettingsId("tblRuns");
	ui->tblRuns->setRowEditorMode(qfw::TableView::EditRowsMixed);
	ui->tblRuns->setInlineEditSaveStrategy(qfw::TableView::OnEditedValueCommit);
	m_runsTableItemDelegate = new RunsTableItemDelegate(ui->tblRuns);
	ui->tblRuns->setItemDelegate(m_runsTableItemDelegate);

	//ui->tblRuns->setSelectionMode(QTableView::SingleSelection);
	ui->tblRuns->viewport()->setAcceptDrops(true);
	ui->tblRuns->setDropIndicatorShown(true);
	ui->tblRuns->setDragDropMode(QAbstractItemView::InternalMove);
	ui->tblRuns->setDragEnabled(false);
	ui->tblRuns->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(ui->tblRuns, &qfw::TableView::customContextMenuRequested, this, &RunsTableWidget::onCustomContextMenuRequest);

	m_runsModel = new RunsTableModel(this);
	connect(m_runsModel, &RunsTableModel::badDataInput, this, &RunsTableWidget::onBadTableDataInput, Qt::QueuedConnection);
	connect(m_runsModel, &RunsTableModel::runnerSiIdEdited, runsPlugin(), &Runs::RunsPlugin::clearRunnersTableCache);
	ui->tblRuns->setTableModel(m_runsModel);

	// this ensures that table is sorted every time when start time is edited
	ui->tblRuns->sortFilterProxyModel()->setDynamicSortFilter(true);

	connect(m_runsModel, &RunsTableModel::startTimesSwitched, [this](int id1, int id2, const QString &err_msg)
	{
		Q_UNUSED(id1)
		Q_UNUSED(id2)
		if(!err_msg.isEmpty()) {
			qf::qmlwidgets::dialogs::MessageBox::showError(this, err_msg);
		}
		//ui->tblRuns->reload(true);
		m_runsModel->reload();
	});

	connect(ui->tblRuns->horizontalHeader(), &QHeaderView::sortIndicatorChanged, [this](int logical_index, Qt::SortOrder order)
	{
		auto cd = m_runsModel->columnDefinition(logical_index);
		bool is_sort_start_time_asc = (cd.matchesSqlId(QStringLiteral("runs.startTimeMs"))
									   && order == Qt::AscendingOrder
									   && ui->tblRuns->filterString().isEmpty());
		m_runsTableItemDelegate->setStartTimeHighlightVisible(is_sort_start_time_asc);
		ui->tblRuns->setDragEnabled(is_sort_start_time_asc);
	});
}

RunsTableWidget::~RunsTableWidget()
{
	delete ui;
}

void RunsTableWidget::clear()
{
	m_runsModel->clearRows();
}

void RunsTableWidget::reload(int stage_id, int class_id, bool show_offrace, const QString &sort_column, int select_competitor_id)
{
	qfLogFuncFrame();
	{
		int class_start_time_min = 0;
		int class_start_interval_min = 0;
		if(class_id > 0) {
			qf::core::sql::Query q;
			q.exec("SELECT startTimeMin, startIntervalMin FROM classdefs WHERE classId=" QF_IARG(class_id) " AND stageId=" QF_IARG(stage_id), qf::core::Exception::Throw);
			if(q.next()) {
				class_start_time_min = q.value(0).toInt();
				class_start_interval_min = q.value(1).toInt();
			}
		}
		ui->lblClassStart->setText(class_start_time_min > 0? QString::number(class_start_time_min): "---");
		ui->lblClassInterval->setText(class_start_interval_min > 0? QString::number(class_start_interval_min): "---");
	}
	qfs::QueryBuilder qb;
	qb.select2("runs", "*")
			.select2("competitors", "id, registration, startNumber, siId, note")
			.select2("classes", "name")
			.select("COALESCE(lastName, '') || ' ' || COALESCE(firstName, '') AS competitorName")
			.select("'' AS disqReason")
			.from("runs")
			.where("runs.stageId=" QF_IARG(stage_id))
			.join("runs.competitorId", "competitors.id")
			.join("competitors.classId", "classes.id")
			.orderBy("runs.id");//.limit(10);
	if(class_id > 0) {
		qb.where("competitors.classId=" + QString::number(class_id));
	}
	if(!show_offrace)
		qb.where("runs.isRunning");
	qfDebug() << qb.toString();
	m_runsTableItemDelegate->setHighlightedClassId(class_id, stage_id);
	m_runsModel->setQueryBuilder(qb, false);
	m_runsModel->reload();

	ui->tblRuns->horizontalHeader()->setSectionHidden(RunsTableModel::col_runs_isRunning, !show_offrace);

	if(!sort_column.isEmpty()) {
		int sort_col_ix = m_runsModel->columnIndex(sort_column);
		if(sort_col_ix >= 0) {
			QHeaderView *hdrv = ui->tblRuns->horizontalHeader();
			hdrv->setSortIndicator(sort_col_ix, Qt::AscendingOrder);
			if(select_competitor_id > 0) {
				for (int i = 0; i < m_runsModel->rowCount(); ++i) {
					int competitor_id = m_runsModel->table().row(i).value(QStringLiteral("competitorId")).toInt();
					if(competitor_id == select_competitor_id) {
						QModelIndex ix = m_runsModel->index(i, sort_col_ix);
						ix = ui->tblRuns->sortFilterProxyModel()->mapFromSource(ix);
						ui->tblRuns->setCurrentIndex(ix);
						//ui->tblRuns->selectionModel()->select(ix, QItemSelectionModel::ClearAndSelect);
						QTimer::singleShot(0, [this, ix]() {
							this->ui->tblRuns->scrollTo(ix);
						});
					}
				}
			}
		}
	}
}

void RunsTableWidget::onCustomContextMenuRequest(const QPoint &pos)
{
	qfLogFuncFrame();
	QAction a_show_receipt(tr("Show receipt"), nullptr);
	QAction a_load_card(tr("Load times from card in selected rows"), nullptr);
	QAction a_print_card(tr("Print card"), nullptr);
	QAction a_sep1(nullptr); a_sep1.setSeparator(true);
	QAction a_shift_start_times(tr("Shift start times in selected rows"), nullptr);
	QList<QAction*> lst;
	lst << &a_show_receipt << &a_load_card << &a_print_card
		<< &a_sep1
		<< &a_shift_start_times;
	QAction *a = QMenu::exec(lst, ui->tblRuns->viewport()->mapToGlobal(pos));
	if(a == &a_load_card) {
		//qf::qmlwidgets::dialogs::MessageBox::showError(this, "Not implemented yet.");
		qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
		qf::qmlwidgets::framework::Plugin *cardreader_plugin = fwk->plugin("CardReader");
		if(!cardreader_plugin) {
			qfError() << "CardReader plugin not installed!";
			return;
		}
		int curr_ix = 0;
		QList<int> sel_ixs = ui->tblRuns->selectedRowsIndexes();
		for(int ix : sel_ixs) {
			qf::core::utils::TableRow row = ui->tblRuns->tableRow(ix);
			int run_id = row.value(QStringLiteral("runs.id")).toInt();
			fwk->showProgress(tr("Reloading times for %1").arg(row.value(QStringLiteral("competitorName")).toString()), ++curr_ix, sel_ixs.count());
			Runs::RunsPlugin *runs_plugin = runsPlugin();
			if(runs_plugin) {
				bool ok = runs_plugin->reloadTimesFromCard(run_id);
				if(ok)
					ui->tblRuns->reloadRow(ix);
			}
		}
		fwk->hideProgress();
	}
	else if(a == &a_show_receipt) {
		int run_id = ui->tblRuns->tableRow().value(QStringLiteral("runs.id")).toInt();
		Runs::RunsPlugin *runs_plugin = runsPlugin();
		if(!runs_plugin)
			return;
		int card_id = runs_plugin->cardForRun(run_id);
		if(card_id > 0) {
			qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
			qf::qmlwidgets::framework::Plugin *receipts_plugin = fwk->plugin("Receipts");
			if(!receipts_plugin) {
				qfError() << "Receipts plugin not installed!";
				return;
			}
			QMetaObject::invokeMethod(receipts_plugin, "previewReceipt", Qt::DirectConnection, Q_ARG(int, card_id));
		}
	}
	else if(a == &a_print_card) {
		int run_id = ui->tblRuns->tableRow().value(QStringLiteral("runs.id")).toInt();
		Runs::RunsPlugin *runs_plugin = runsPlugin();
		if(!runs_plugin)
			return;
		int card_id = runs_plugin->cardForRun(run_id);
		if(card_id > 0) {
			qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
			qf::qmlwidgets::framework::Plugin *receipts_plugin = fwk->plugin("Receipts");
			if(!receipts_plugin) {
				qfError() << "Receipts plugin not installed!";
				return;
			}
			QMetaObject::invokeMethod(receipts_plugin, "printReceipt", Qt::DirectConnection, Q_ARG(int, card_id));
		}
	}
	else if(a == &a_shift_start_times) {
		try {
			int offset_msec = 0;
			int interval = ui->lblClassInterval->text().toInt();
			if(interval == 0)
				interval = 1;
			bool ok;
			offset_msec = QInputDialog::getInt(this, tr("Get number"), tr("Start times offset [min]:"), 0, -1000, 1000, interval, &ok);
			if(ok) {
				offset_msec *= 60 * 1000;

				qfs::Transaction transaction;
				qfs::Query q(transaction.connection());
				q.prepare("UPDATE runs SET startTimeMs = COALESCE(startTimeMs, 0) + :offset WHERE id=:id", qf::core::Exception::Throw);
				QList<int> rows = ui->tblRuns->selectedRowsIndexes();
				for(int ix : rows) {
					qf::core::utils::TableRow row = ui->tblRuns->tableRow(ix);
					int id = row.value(ui->tblRuns->idColumnName()).toInt();
					q.bindValue(QStringLiteral(":offset"), offset_msec);
					q.bindValue(QStringLiteral(":id"), id);
					//qfInfo() << id << "->" << offset_msec;
					q.exec(qf::core::Exception::Throw);
				}
				transaction.commit();
				runsModel()->reload();
			}
		}
		catch (const qf::core::Exception &e) {
			qf::qmlwidgets::dialogs::MessageBox::showException(this, e);
		}
	}
}

void RunsTableWidget::onTableViewSqlException(const QString &what, const QString &where, const QString &stack_trace)
{
	if(what.contains(QLatin1String("runs.stageId")) && what.contains(QLatin1String("runs.siId"))) {
		// "UNIQUE constraint failed: runs.stageId, runs.siId Unable to fetch row"
		// duplicate SI insertion attempt
		qf::qmlwidgets::dialogs::MessageBox::showError(this, tr("Duplicate SI inserted."));
		return;
	}
	qf::qmlwidgets::dialogs::MessageBox::showException(this, what, where, stack_trace);
}

void RunsTableWidget::onBadTableDataInput(const QString &message)
{
	qf::qmlwidgets::dialogs::MessageBox::showError(this, message);
}


