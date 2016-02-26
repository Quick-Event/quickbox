#include "eventstatisticswidget.h"
#include "ui_eventstatisticswidget.h"

#include <Event/eventplugin.h>

#include <qf/core/model/sqltablemodel.h>
#include <qf/core/sql/querybuilder.h>
#include <qf/qmlwidgets/framework/mainwindow.h>

#include <QTimer>

namespace qfs = qf::core::sql;
namespace qfu = qf::core::utils;

static Event::EventPlugin* eventPlugin()
{
	qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
	qf::qmlwidgets::framework::Plugin *plugin = fwk->plugin("Event");
	QF_ASSERT_EX(plugin != nullptr, "Bad Event plugin!");
	return qobject_cast<Event::EventPlugin*>(plugin);
}

//============================================================
//                EventStatisticsModel
//============================================================
class EventStatisticsModel : public qf::core::model::SqlTableModel
{
	typedef qf::core::model::SqlTableModel Super;
public:
	EventStatisticsModel(QObject *parent);

	QVariant data(const QModelIndex &index, int role) const Q_DECL_OVERRIDE;
	QVariant value(int row_ix, int column_ix) const Q_DECL_OVERRIDE;
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

QVariant EventStatisticsModel::value(int row_ix, int column_ix) const
{
	static int col_map_count = columnIndex(QStringLiteral("classdefs.mapCount"));
	static int col_free_map_count = columnIndex(QStringLiteral("freeMapCount"));
	static int col_runners_count = columnIndex(QStringLiteral("runnersCount"));
	static int col_runners_finished = columnIndex(QStringLiteral("runnersFinished"));
	static int col_runners_not_finished = columnIndex(QStringLiteral("runnersNotFinished"));
	if(column_ix == col_free_map_count) {
		int cnt = value(row_ix, col_map_count).toInt() - value(row_ix, col_runners_count).toInt();
		return cnt;
	}
	else if(column_ix == col_runners_not_finished) {
		int cnt = value(row_ix, col_runners_count).toInt() - value(row_ix, col_runners_finished).toInt();
		return cnt;
	}
	return Super::value(row_ix, column_ix);
}

QVariant EventStatisticsModel::data(const QModelIndex &index, int role) const
{
	int col = index.column();
	static int col_free_map_count = columnIndex(QStringLiteral("freeMapCount"));
	if(role == Qt::BackgroundRole) {
		if(col == col_free_map_count) {
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
	auto *mm = masterModel();
	m_columnSums.resize(columnCount());
	for (int i = 0; i < columnCount(); ++i) {
		int type = mm->columnType(i);
		//qfInfo() << type << "type:" << QVariant::typeToName(type);
		int isum = 0;
		double dsum = 0;
		bool int_col = (type == QVariant::Int || type == QVariant::UInt);
		bool double_col = (type == QVariant::Double);
		for (int j = 0; j < mm->rowCount(); ++j) {
			//QModelIndex ix = mm->index(j, i);
			if(int_col)
				isum += mm->value(j, i).toInt();
			else if(double_col)
				dsum += mm->value(j, i).toDouble();
		}
		if(int_col)
			m_columnSums[i] = isum;
		else if(double_col)
			m_columnSums[i] = dsum;
		else
			m_columnSums[i] = QVariant();
	}
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
	ui->tableLayout->addWidget(m_tableFooterView);

	//connect(ui->tableView->horizontalHeader(), &QHeaderView::sectionResized, this, &EventStatisticsWidget::onSectionResized);
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
		m_tableFooterModel = new FooterModel(this);
		m_tableFooterModel->setMasterModel(m_tableModel);

		ui->tableView->setTableModel(m_tableModel);
		ui->tableView->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);
		m_tableFooterView->setModel(m_tableFooterModel);
	}
	QVariantMap qm;
	qm[QStringLiteral("stage_id")] = currentStageId();
	m_tableModel->setQueryParameters(qm);
	m_tableModel->reload();
	//m_tableModel->recentlyExecutedQuery();
	QTimer::singleShot(10, m_tableFooterView, &FooterView::syncSectionSizes);
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
