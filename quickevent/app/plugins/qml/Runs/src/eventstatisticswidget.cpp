#include "eventstatisticswidget.h"
#include "ui_eventstatisticswidget.h"

#include <Event/eventplugin.h>

#include <qf/core/model/sqltablemodel.h>
#include <qf/core/sql/querybuilder.h>
#include <qf/qmlwidgets/framework/mainwindow.h>

static Event::EventPlugin* eventPlugin()
{
	qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
	qf::qmlwidgets::framework::Plugin *plugin = fwk->plugin("Event");
	QF_ASSERT_EX(plugin != nullptr, "Bad Event plugin!");
	return qobject_cast<Event::EventPlugin*>(plugin);
}

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
		m_tableModel = new qf::core::model::SqlTableModel(this);
		m_tableModel->addColumn("classes.name", tr("Class"));
		m_tableModel->addColumn("classdefs.mapCount", tr("Maps"));
		m_tableModel->addColumn("runnersCount", tr("Runners"));
		m_tableModel->addColumn("runnersFinished", tr("Finished"));
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
					.from("classes")
					.joinRestricted("classes.id", "classdefs.classId", "classdefs.stageId={{stage_id}}")
					//.join("classes.id", "competitors.classId")
					//.joinRestricted("competitors.id", "runs.competitorId", "runs.stageId={{stage_id}}")
					//.groupBy("classes.id")
					.orderBy("classes.name");
			m_tableModel->setQueryBuilder(qb);
		}

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
