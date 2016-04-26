#include "eventstatisticswidget.h"
#include "ui_eventstatisticswidget.h"

#include "Runs/reportoptionsdialog.h"
#include "Runs/runsplugin.h"

#include <Event/eventplugin.h>

#include <quickevent/og/sqltablemodel.h>
#include <quickevent/og/timems.h>

#include <qf/core/sql/querybuilder.h>
#include <qf/qmlwidgets/framework/mainwindow.h>
#include <qf/qmlwidgets/reports/widgets/reportviewwidget.h>

#include <qf/core/assert.h>

#include <QElapsedTimer>
#include <QTimer>

namespace qfs = qf::core::sql;
namespace qfu = qf::core::utils;

static Event::EventPlugin* eventPlugin()
{
	qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
	auto *plugin = qobject_cast<Event::EventPlugin*>(fwk->plugin("Event"));
	QF_ASSERT_EX(plugin != nullptr, "Bad Event plugin!");
	return plugin;
}

static Runs::RunsPlugin* runsPlugin()
{
	qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
	auto *plugin = qobject_cast<Runs::RunsPlugin *>(fwk->plugin("Runs"));
	QF_ASSERT_EX(plugin != nullptr, "Bad Runs plugin!");
	return plugin;
}

//============================================================
//                EventStatisticsModel
//============================================================
class EventStatisticsModel : public quickevent::og::SqlTableModel
{
	typedef quickevent::og::SqlTableModel Super;
public:
	enum Cols {
		col_classes_name,
		col_mapCount,
		col_freeMapCount,
		col_runnersCount,
		col_startFirstMs,
		col_startLastMs,
		col_time1Ms,
		col_time3Ms,
		col_timeToCloseMs,
		col_runnersFinished,
		col_runnersNotFinished,
		col_resultsNotPrinted,
		col_COUNT
	};
public:
	EventStatisticsModel(QObject *parent);

	QVariant data(const QModelIndex &index, int role) const Q_DECL_OVERRIDE;
	QVariant value(int row_ix, int column_ix) const Q_DECL_OVERRIDE;
};

EventStatisticsModel::EventStatisticsModel(QObject *parent)
	: Super(parent)
{
	clearColumns(col_COUNT);
	setColumn(col_classes_name, ColumnDefinition("classes.name", tr("Class")));
	setColumn(col_mapCount, ColumnDefinition("classdefs.mapCount", tr("Maps")));
	setColumn(col_freeMapCount, ColumnDefinition("freeMapCount", tr("Free maps")));
	setColumn(col_runnersCount, ColumnDefinition("runnersCount", tr("Runners")));
	setColumn(col_startFirstMs, ColumnDefinition("startFirstMs", tr("Start first")).setCastType(qMetaTypeId<quickevent::og::TimeMs>()));
	setColumn(col_startLastMs, ColumnDefinition("startLastMs", tr("Start last")).setCastType(qMetaTypeId<quickevent::og::TimeMs>()));
	setColumn(col_time1Ms, ColumnDefinition("time1Ms", tr("Time 1")).setToolTip(tr("Finish time of first runner in current class")).setCastType(qMetaTypeId<quickevent::og::TimeMs>()));
	setColumn(col_time3Ms, ColumnDefinition("time3Ms", tr("Time 3")).setToolTip(tr("Finish time of third runner in current class")).setCastType(qMetaTypeId<quickevent::og::TimeMs>()));
	setColumn(col_timeToCloseMs, ColumnDefinition("timeToCloseMs", tr("Time to close")).setToolTip(tr("Time to class close")).setCastType(qMetaTypeId<quickevent::og::TimeMs>()));
	setColumn(col_runnersFinished, ColumnDefinition("runnersFinished", tr("Finished")));
	setColumn(col_runnersNotFinished, ColumnDefinition("runnersNotFinished", tr("Not finished")));
	setColumn(col_resultsNotPrinted, ColumnDefinition("resultsNotPrinted", tr("New results")).setToolTip(tr("Number of finished competitors not printed in results.")));
	{
		static const auto competiting_cond = QStringLiteral("runs.stageId={{stage_id}} AND NOT runs.offRace AND competitors.classId=classes.id");
		qf::core::sql::QueryBuilder qb_runners_count;
		qb_runners_count.select("COUNT(runs.id)")
				.from("runs").joinRestricted("runs.competitorId", "competitors.id", competiting_cond, qf::core::sql::QueryBuilder::INNER_JOIN);
		qf::core::sql::QueryBuilder qb_runners_finished;
		qb_runners_finished.select("COUNT(runs.id)")
				.from("runs").joinRestricted("runs.competitorId", "competitors.id", competiting_cond, qf::core::sql::QueryBuilder::INNER_JOIN)
				.where("runs.finishTimeMs > 0 OR runs.disqualified");
		qf::core::sql::QueryBuilder qb_runners_start_first;
		qb_runners_start_first.select("MIN(runs.startTimeMs)")
				.from("runs").joinRestricted("runs.competitorId", "competitors.id", competiting_cond, qf::core::sql::QueryBuilder::INNER_JOIN)
				.where("runs.startTimeMs IS NOT NULL");
		qf::core::sql::QueryBuilder qb_runners_start_last;
		qb_runners_start_last.select("MAX(runs.startTimeMs)")
				.from("runs").joinRestricted("runs.competitorId", "competitors.id", competiting_cond, qf::core::sql::QueryBuilder::INNER_JOIN)
				.where("runs.startTimeMs IS NOT NULL");
		static const auto in_results_cond = competiting_cond + QStringLiteral(" AND runs.timeMs>0 AND NOT runs.disqualified");
		qf::core::sql::QueryBuilder qb_first_time;
		{
			qf::core::sql::QueryBuilder qb;
			qb.select("runs.timeMs")
				.from("runs").joinRestricted("runs.competitorId", "competitors.id", in_results_cond, qf::core::sql::QueryBuilder::INNER_JOIN)
				.orderBy("runs.timeMs")
				.limit(1)
				.as("results1");
			qb_first_time.select("MAX(timeMs)")
					.from(qb.toString());
		}
		qf::core::sql::QueryBuilder qb_third_time;
		{
			qf::core::sql::QueryBuilder qb;
			qb.select("runs.timeMs")
				.from("runs").joinRestricted("runs.competitorId", "competitors.id", in_results_cond, qf::core::sql::QueryBuilder::INNER_JOIN)
				.orderBy("runs.timeMs")
				.limit(3)
				.as("results3");
			qb_third_time.select("MAX(timeMs)")//.select("COUNT(timeMs) AS count3")
					.from(qb.toString());
		}

		qf::core::sql::QueryBuilder qb;
		qb.select2("classes", "*")
				.select2("classdefs", "*")
				.select("(" + qb_runners_count.toString() + ") AS runnersCount")
				.select("(" + qb_runners_finished.toString() + ") AS runnersFinished")
				.select("(" + qb_runners_start_first.toString() + ") AS startFirstMs")
				.select("(" + qb_runners_start_last.toString() + ") AS startLastMs")
				.select("(" + qb_first_time.toString() + ") AS time1Ms")
				.select("(" + qb_third_time.toString() + ") AS time3Ms")
				.select("0 AS timeToCloseMs")
				.select("0 AS freeMapCount")
				.select("0 AS runnersNotFinished")
				.select("0 AS resultsNotPrinted")
				.from("classes")
				.joinRestricted("classes.id", "classdefs.classId", "classdefs.stageId={{stage_id}}")
				//.join("classes.id", "competitors.classId")
				//.joinRestricted("competitors.id", "runs.competitorId", "runs.stageId={{stage_id}}")
				//.groupBy("classes.id")
				.orderBy("classes.name");
		setQueryBuilder(qb);
	}
}

QVariant EventStatisticsModel::value(int row_ix, int column_ix) const
{
	if(column_ix == col_freeMapCount) {
		int cnt = value(row_ix, col_mapCount).toInt() - value(row_ix, col_runnersCount).toInt();
		return cnt;
	}
	if(column_ix == col_timeToCloseMs) {
		int stage_start_msec = eventPlugin()->stageStart(eventPlugin()->currentStageId());
		int curr_time_msec = QTime::currentTime().msecsSinceStartOfDay();
		int start_last_msec = value(row_ix, col_startLastMs).toInt();
		int time3_msec = value(row_ix, col_time3Ms).toInt();
		int time_to_close_msec = stage_start_msec + start_last_msec + time3_msec - curr_time_msec;
		if(time_to_close_msec < 0)
			time_to_close_msec = 0;
		return time_to_close_msec;
	}
	else if(column_ix == col_runnersNotFinished) {
		int cnt = value(row_ix, col_runnersCount).toInt() - value(row_ix, col_runnersFinished).toInt();
		return cnt;
	}
	else if(column_ix == col_resultsNotPrinted) {
		int results_count = tableRow(row_ix).value(QStringLiteral("resultsCount")).toInt();
		int cnt = value(row_ix, col_runnersFinished).toInt() - results_count;
		return cnt;
	}
	return Super::value(row_ix, column_ix);
}

QVariant EventStatisticsModel::data(const QModelIndex &index, int role) const
{
	int col = index.column();
	if(role == Qt::BackgroundRole) {
		if(col == col_freeMapCount) {
			int cnt = data(index, Qt::DisplayRole).toInt();
			if(cnt < 0)
				return QColor(Qt::red);
			return QVariant();
		}
	}
	return Super::data(index, role);
}

//============================================================
//                FooterModel
//============================================================
class FooterModel : public QAbstractTableModel
{
	typedef QAbstractTableModel Super;
public:
	FooterModel(QObject *parent);

	int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
	int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

	qf::core::model::TableModel* masterModel() const;
	void setMasterModel(qf::core::model::TableModel *masterModel);

	void reload();
private:
	qf::core::model::TableModel *m_masterModel = nullptr;
	QVector<QVariant> m_columnSums;
};

FooterModel::FooterModel(QObject *parent)
	: Super(parent)
{
}

int FooterModel::rowCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent)
	return 0;
}

int FooterModel::columnCount(const QModelIndex &parent) const
{
	return masterModel()->columnCount(parent);
}

QVariant FooterModel::data(const QModelIndex &index, int role) const
{
	Q_UNUSED(index)
	Q_UNUSED(role)
	return QVariant();
}

QVariant FooterModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if(orientation == Qt::Horizontal) {
		if(role == Qt::DisplayRole) {
			return m_columnSums.value(section);
		}
		else if(role == Qt::TextAlignmentRole) {
			return Qt::AlignRight;
		}
		else if(role == Qt::BackgroundRole) {
			return QColor("khaki");
		}
	}
	return Super::headerData(section, orientation, role);
}

qf::core::model::TableModel *FooterModel::masterModel() const
{
	QF_ASSERT_EX(m_masterModel != nullptr, "Master model is NULL");
	return m_masterModel;
}

void FooterModel::setMasterModel(qf::core::model::TableModel *masterModel)
{
	m_masterModel = masterModel;
	connect(m_masterModel, &qf::core::model::SqlTableModel::reloaded, this, &FooterModel::reload);
}

void FooterModel::reload()
{
	qfLogFuncFrame();
	auto *mm = masterModel();
	m_columnSums.resize(columnCount());
	for (int i = 0; i < columnCount(); ++i) {
		int type = mm->columnType(i);
		//qfInfo() << type << "type:" << QVariant::typeToName(type);
		int isum = 0;
		double dsum = 0;
		bool int_col = (type == QVariant::Int || type == QVariant::UInt || type == QVariant::LongLong || type == QVariant::ULongLong);
		bool double_col = (type == QVariant::Double);
		for (int j = 0; j < mm->rowCount(); ++j) {
			//QModelIndex ix = mm->index(j, i);
			if(int_col) {
				if(i == 2)
					qfDebug() << mm->value(j, i);
				isum += mm->value(j, i).toInt();
			}
			else if(double_col) {
				dsum += mm->value(j, i).toDouble();
			}
		}
		if(int_col)
			m_columnSums[i] = isum;
		else if(double_col)
			m_columnSums[i] = dsum;
		else
			m_columnSums[i] = QVariant();
		if(i == 2)
			qfDebug() << "SUM:" << m_columnSums[i];
	}
	emit headerDataChanged(Qt::Horizontal, 0, columnCount() - 1);
}

//============================================================
//                FooterView
//============================================================
class FooterView : public QHeaderView
{
	typedef QHeaderView Super;
public:
	FooterView(QTableView *table_view, QWidget *parent = nullptr);

	void syncSectionSizes();
private:
	QTableView *m_tableView;
};

FooterView::FooterView(QTableView *table_view, QWidget *parent)
	: Super(Qt::Horizontal, parent)
	, m_tableView(table_view)
{
	setSectionResizeMode(QHeaderView::Fixed);
	//QHeaderView *vh = table_view->verticalHeader();
	QHeaderView *hh = table_view->horizontalHeader();
	if(hh) {
		//setMaximumHeight(hh->defaultSectionSize());
		connect(hh, &QHeaderView::sectionResized, [this](int logical_index, int old_size, int new_size) {
			Q_UNUSED(old_size)
			this->resizeSection(logical_index, new_size);
		});
	}
}

void FooterView::syncSectionSizes()
{
	QAbstractItemModel *m = model();
	QF_ASSERT(m != nullptr, "Model is NULL!", return);

	QHeaderView *vh = m_tableView->verticalHeader();
	if(vh) {
		int w = vh->width();
		setViewportMargins(w, 0, 0, 0);
		setMaximumHeight(vh->defaultSectionSize());
	}
	QHeaderView *hh = m_tableView->horizontalHeader();
	if(hh) {
		for (int i = 0; i < hh->count() && i < m->columnCount(); ++i) {
			resizeSection(i, hh->sectionSize(i));
		}
	}
}

//============================================================
//                EventStatisticsWidget
//============================================================
EventStatisticsWidget::EventStatisticsWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::EventStatisticsWidget)
{
	ui->setupUi(this);
	ui->tableView->setPersistentSettingsId("tblEventStatistics");
	ui->tableView->setReadOnly(true);

	m_tableFooterView = new FooterView(ui->tableView);
	m_tableFooterView->setMinimumHeight(20);
	ui->tableLayout->addWidget(m_tableFooterView);

	connect(eventPlugin(), SIGNAL(dbEventNotify(QString,QVariant)), this, SLOT(onDbEventNotify(QString,QVariant)), Qt::QueuedConnection);
	connect(eventPlugin(), &Event::EventPlugin::currentStageIdChanged, this, &EventStatisticsWidget::reload);
}

EventStatisticsWidget::~EventStatisticsWidget()
{
	delete ui;
}

void EventStatisticsWidget::reloadLater()
{
	QTimer *tm = reloadLaterTimer();
	if(!tm->isActive())
		tm->start();
}

void EventStatisticsWidget::reload()
{
	qfLogFuncFrame() << isVisible();
	if(!isVisible())
		return;

	int stage_id = currentStageId();
	if(stage_id == 0)
		return;

	if(!m_tableModel) {
		m_tableModel = new EventStatisticsModel(this);
		m_tableFooterModel = new FooterModel(this);
		m_tableFooterModel->setMasterModel(m_tableModel);

		ui->tableView->setTableModel(m_tableModel);
		ui->tableView->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);
		m_tableFooterView->setModel(m_tableFooterModel);
	}
	QVariantMap qm;
	qm[QStringLiteral("stage_id")] = currentStageId();
	m_tableModel->setQueryParameters(qm);
	QElapsedTimer tm;
	tm.start();
	m_tableModel->reload();
	qfDebug() << m_tableModel->effectiveQuery();
	qfDebug() << "query execution time:" << tm.elapsed() << "msec";
	QTimer::singleShot(10, m_tableFooterView, &FooterView::syncSectionSizes);
}

void EventStatisticsWidget::onDbEventNotify(const QString &domain, const QVariant &payload)
{
	Q_UNUSED(payload)
	if(domain == QLatin1String(Event::EventPlugin::DBEVENT_CARD_READ)
	   || domain == QLatin1String(Event::EventPlugin::DBEVENT_COMPETITOR_COUNTS_CHANGED)) {
		reloadLater();
	}
}

void EventStatisticsWidget::onVisibleChanged(bool is_visible)
{
	Q_UNUSED(is_visible)
	reload();
}

int EventStatisticsWidget::currentStageId()
{
	int ret = eventPlugin()->currentStageId();
	return ret;
}

QTimer *EventStatisticsWidget::reloadLaterTimer()
{
	if(!m_reloadLaterTimer) {
		m_reloadLaterTimer = new QTimer(this);
		m_reloadLaterTimer->setInterval(1000);
		m_reloadLaterTimer->setSingleShot(true);
		connect(m_reloadLaterTimer, &QTimer::timeout, this, &EventStatisticsWidget::reload);
	}
	return m_reloadLaterTimer;
}

void EventStatisticsWidget::on_btReload_clicked()
{
	qfLogFuncFrame();
	reload();
}

void EventStatisticsWidget::on_btPrintResults_clicked()
{
	QStringList class_names;
	QList<int> classdefs_ids;
	QList<int> runners_finished;
	for(int i : ui->tableView->selectedRowsIndexes()) {
		qf::core::utils::TableRow row = ui->tableView->tableRow(i);
		class_names << row.value(QStringLiteral("classes.name")).toString();
		classdefs_ids << row.value(QStringLiteral("classdefs.id")).toInt();
		runners_finished << row.value(QStringLiteral("runnersFinished")).toInt();
	}
	bool report_printed = false;
	Runs::ReportOptionsDialog dlg(this);
	dlg.setClassNamesFilter(class_names);
	if(dlg.exec()) {
		QVariant td = runsPlugin()->currentStageResultsTableData(dlg.sqlWhereExpression());
		QVariantMap props;
		props["isBreakAfterEachClass"] = dlg.isBreakAfterEachClass();
		props["isColumnBreak"] = dlg.isColumnBreak();
		report_printed = qf::qmlwidgets::reports::ReportViewWidget::showReport(this
									, runsPlugin()->manifest()->homeDir() + "/reports/results_stage.qml"
									, td
									, tr("Results by clases")
									, "printCurrentStage"
									, props
									);
	}
	if(report_printed) {
		clerNewResults(classdefs_ids, runners_finished);
		reload();
	}
}

void EventStatisticsWidget::on_btClearNewInSelectedRows_clicked()
{
	qfLogFuncFrame();
	QList<int> classdefs_ids;
	QList<int> runners_finished;
	QList<int> sel_rows = ui->tableView->selectedRowsIndexes();
	if(sel_rows.isEmpty())
		ui->tableView->selectAll();
	sel_rows = ui->tableView->selectedRowsIndexes();
	for(int i : sel_rows) {
		qf::core::utils::TableRow row = ui->tableView->tableRow(i);
		//class_names << row.value(QStringLiteral("classes.name")).toString();
		classdefs_ids << row.value(QStringLiteral("classdefs.id")).toInt();
		runners_finished << row.value(QStringLiteral("runnersFinished")).toInt();
	}
	clerNewResults(classdefs_ids, runners_finished);
	reload();
}

void EventStatisticsWidget::clerNewResults(const QList<int> &classdefs_ids, const QList<int> &runners_finished)
{
	qfLogFuncFrame();
	QString qs = "UPDATE classdefs SET resultsCount=:resultsCount WHERE id=:id";
	qf::core::sql::Query q;
	q.prepare(qs, qf::core::Exception::Throw);
	for (int i = 0; i < classdefs_ids.count(); ++i) {
		qfDebug() << classdefs_ids[i] << runners_finished[i];
		q.bindValue(":resultsCount", runners_finished[i]);
		q.bindValue(":id", classdefs_ids[i]);
		q.exec(qf::core::Exception::Throw);
	}
}

