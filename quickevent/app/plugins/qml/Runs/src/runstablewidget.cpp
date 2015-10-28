#include "runstablewidget.h"
#include "ui_runstablewidget.h"
#include "runstablemodel.h"
#include "runstableitemdelegate.h"
#include "Runs/runsplugin.h"

#include <quickevent/og/siid.h>
#include <quickevent/og/timems.h>

#include <qf/qmlwidgets/dialogs/messagebox.h>
#include <qf/qmlwidgets/framework/mainwindow.h>
#include <qf/qmlwidgets/framework/plugin.h>

#include <qf/core/log.h>

#include <QSortFilterProxyModel>
#include <QMenu>
#include <QTimer>

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

RunsTableWidget::RunsTableWidget(QWidget *parent) :
	Super(parent),
	ui(new Ui::RunsTableWidget)
{
	ui->setupUi(this);

	ui->tblRunsToolBar->setTableView(ui->tblRuns);

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

	auto m = new RunsTableModel(this);
	m->addColumn("runs.id").setReadOnly(true);
	m->addColumn("classes.name", tr("Class"));
	m->addColumn("competitors.siId", tr("SI"));
	m->addColumn("competitorName", tr("Name"));
	m->addColumn("registration", tr("Reg"));
	m->addColumn("runs.siId", tr("SI")).setCastType(qMetaTypeId<quickevent::og::SiId>());
	m->addColumn("runs.startTimeMs", tr("Start")).setCastType(qMetaTypeId<quickevent::og::TimeMs>());
	m->addColumn("runs.timeMs", tr("Time")).setCastType(qMetaTypeId<quickevent::og::TimeMs>());
	m->addColumn("runs.finishTimeMs", tr("Finish")).setCastType(qMetaTypeId<quickevent::og::TimeMs>());
	m->addColumn("runs.notCompeting", tr("NC")).setToolTip(tr("Not competing"));
	m->addColumn("runs.cardLent", tr("L")).setToolTip(tr("Card lent"));
	m->addColumn("runs.cardReturned", tr("R")).setToolTip(tr("Card returned"));
	m->addColumn("runs.misPunch", tr("Error")).setToolTip(tr("Card mispunch")).setReadOnly(true);
	m->addColumn("runs.disqualified", tr("DISQ")).setToolTip(tr("Disqualified"));
	m->addColumn("competitors.note", tr("Note"));
	/*
	qfm::SqlTableModel::ColumnDefinition::DbEnumCastProperties status_props;
	status_props.setGroupName("runs.status");
	m->addColumn("runs.status", tr("Status")).setCastType(qMetaTypeId<qf::core::sql::DbEnum>(), status_props);
	*/
	ui->tblRuns->setTableModel(m);
	m_runsModel = m;

	// this ensures that table is sorted every time when start time is edited
	ui->tblRuns->sortFilterProxyModel()->setDynamicSortFilter(true);

	connect(m_runsModel, &RunsTableModel::startTimesSwitched, ui->tblRuns, [this](int id1, int id2, const QString &err_msg)
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

void RunsTableWidget::reload(int stage_id, int class_id, const QString &sort_column, int select_competitor_id)
{
	qfLogFuncFrame();
	qfs::QueryBuilder qb;
	qb.select2("runs", "*")
			.select2("competitors", "registration, siId, note")
			.select2("classes", "name")
			.select("COALESCE(lastName, '') || ' ' || COALESCE(firstName, '') AS competitorName")
			.from("runs")
			.where("NOT runs.offRace")
			.where("runs.stageId=" QF_IARG(stage_id))
			.join("runs.competitorId", "competitors.id")
			.join("competitors.classId", "classes.id")
			.orderBy("runs.id");//.limit(10);
	if(class_id > 0) {
		qb.where("competitors.classId=" + QString::number(class_id));
	}
	qfDebug() << qb.toString();
	m_runsTableItemDelegate->setHighlightedClassId(class_id);
	m_runsModel->setQueryBuilder(qb);
	m_runsModel->reload();
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

void RunsTableWidget::onCustomContextMenuRequest(const QPoint &pos)
{
	qfLogFuncFrame();
	QAction a_show_card(tr("Show card"), nullptr);
	QAction a_load_card(tr("Load times from card in selected rows"), nullptr);
	QAction a_print_card(tr("Print card"), nullptr);
	QList<QAction*> lst;
	lst << &a_show_card << &a_load_card << &a_print_card;
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
				int card_id = runs_plugin->cardForRun(run_id);
				bool ok;
				QMetaObject::invokeMethod(cardreader_plugin, "reloadTimesFromCard", Qt::DirectConnection,
				                          Q_RETURN_ARG(bool, ok),
				                          Q_ARG(int, card_id),
										  Q_ARG(int, run_id));
				//QF_ASSERT(ok == true, "reloadTimesFromCard error!", break);
				if(ok)
					ui->tblRuns->reloadRow(ix);
			}
		}
		fwk->hideProgress();
	}
	else if(a == &a_show_card) {
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
}

