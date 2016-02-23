#include "eventstatisticswidget.h"
#include "ui_eventstatisticswidget.h"

#include <Event/eventplugin.h>

#include <qf/core/model/sqltablemodel.h>
#include <qf/core/sql/querybuilder.h>
#include <qf/qmlwidgets/framework/mainwindow.h>

namespace qfs = qf::core::sql;
namespace qfu = qf::core::utils;

static Event::EventPlugin* eventPlugin()
{
	qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
	qf::qmlwidgets::framework::Plugin *plugin = fwk->plugin("Event");
	QF_ASSERT_EX(plugin != nullptr, "Bad Event plugin!");
	return qobject_cast<Event::EventPlugin*>(plugin);
}

class EventStatisticsModel : public qf::core::model::SqlTableModel
{
	typedef qf::core::model::SqlTableModel Super;
public:
	EventStatisticsModel(QObject *parent);

	QVariant data(const QModelIndex &index, int role) const Q_DECL_OVERRIDE;
protected:
	bool reloadTable(const QString &query_str) Q_DECL_OVERRIDE;
};

EventStatisticsModel::EventStatisticsModel(QObject *parent)
	: Super(parent)
{
	addColumn("classes.name", tr("Class"));
	addColumn("classdefs.mapCount", tr("Maps"));
	addColumn("freeMapCount", tr("Free maps"));
	addColumn("runnersCount", tr("Runners"));
	addColumn("runnersFinished", tr("Finished"));
	addColumn("runnersNotFinished", tr("Not finished"));
	{
		qf::core::sql::QueryBuilder qb_runners_count;
		qb_runners_count.select("COUNT(runs.id)")
				.from("runs").joinRestricted("runs.competitorId", "competitors.id", "runs.stageId={{stage_id}} AND competitors.classId=classes.id", qf::core::sql::QueryBuilder::INNER_JOIN);
		qf::core::sql::QueryBuilder qb_runners_finished;
		qb_runners_finished.select("COUNT(runs.id)")
				.from("runs").joinRestricted("runs.competitorId", "competitors.id", "runs.stageId={{stage_id}} AND competitors.classId=classes.id", qf::core::sql::QueryBuilder::INNER_JOIN)
				.where("runs.finishTimeMs > 0");
		qf::core::sql::QueryBuilder qb;
		qb.select2("classes", "*")
				.select2("classdefs", "*")
				.select("(" + qb_runners_count.toString() + ") AS runnersCount")
				.select("(" + qb_runners_finished.toString() + ") AS runnersFinished")
				.select("0 AS freeMapCount")
				.select("0 AS runnersNotFinished")
				.from("classes")
				.joinRestricted("classes.id", "classdefs.classId", "classdefs.stageId={{stage_id}}")
				//.join("classes.id", "competitors.classId")
				//.joinRestricted("competitors.id", "runs.competitorId", "runs.stageId={{stage_id}}")
				//.groupBy("classes.id")
				.orderBy("classes.name");
		setQueryBuilder(qb);
	}
}

QVariant EventStatisticsModel::data(const QModelIndex &index, int role) const
{
	int col = index.column();
	static int col_map_count = columnIndex(QStringLiteral("classdefs.mapCount"));
	static int col_free_map_count = columnIndex(QStringLiteral("freeMapCount"));
	static int col_runners_count = columnIndex(QStringLiteral("runnersCount"));
	static int col_runners_finished = columnIndex(QStringLiteral("runnersFinished"));
	static int col_runners_not_finished = columnIndex(QStringLiteral("runnersNotFinished"));
	if(role == Qt::DisplayRole) {
		if(col == col_free_map_count) {
			int cnt = value(index.row(), col_map_count).toInt() - value(index.row(), col_runners_count).toInt();
			return cnt;
		}
		else if(col == col_runners_not_finished) {
			int cnt = value(index.row(), col_runners_count).toInt() - value(index.row(), col_runners_finished).toInt();
			return cnt;
		}
	}
	else if(role == Qt::BackgroundRole) {
		if(index.row() == rowCount() - 1) {
			return QColor("khaki");
		}
		else {
			if(col == col_free_map_count) {
				int cnt = data(index, Qt::DisplayRole).toInt();
				if(cnt < 0)
					return QColor(Qt::red);
				return QVariant();
			}
		}
	}
	return Super::data(index, role);
}

bool EventStatisticsModel::reloadTable(const QString &query_str)
{
	bool ret = Super::reloadTable(query_str);
	if(ret && m_recentlyExecutedQuery.isSelect()) {
		qfu::TableRow &row = m_table.appendRow();
		row.setInsert(false);
		for(int i=0; i<columnCount(); i++) {
			int type = columnType(i);
			if(type == QVariant::Int) {
				int sum = 0;
				for (int j = 0; j < m_table.rowCount() - 1; ++j) {
					int val = m_table.row(j).value(i).toInt();
					sum += val;
				}
				row.setBareBoneValue(i, sum);
			}
		}
	}
	return ret;
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

	connect(eventPlugin(), &Event::EventPlugin::currentStageIdChanged, this, &EventStatisticsWidget::reload);
}

EventStatisticsWidget::~EventStatisticsWidget()
{
	delete ui;
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

		ui->tableView->setTableModel(m_tableModel);
		ui->tableView->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);
	}
	QVariantMap qm;
	qm[QStringLiteral("stage_id")] = currentStageId();
	m_tableModel->setQueryParameters(qm);
	m_tableModel->reload();
	m_tableModel->recentlyExecutedQuery();
}

void EventStatisticsWidget::onDbEvent(const QString &domain, const QVariant &payload)
{

}

int EventStatisticsWidget::currentStageId()
{
	int ret = eventPlugin()->currentStageId();
	return ret;
}

void EventStatisticsWidget::on_btReload_clicked()
{
	reload();
}
