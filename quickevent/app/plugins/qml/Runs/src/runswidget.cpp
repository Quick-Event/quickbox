#include "runswidget.h"
#include "ui_runswidget.h"
#include "thispartwidget.h"
#include "runstablemodel.h"
#include "runstableitemdelegate.h"
#include "Runs/runsplugin.h"

#include <Event/eventplugin.h>

//#include <quickevent/og/siid.h>
//#include <quickevent/og/timems.h>
#include <quickevent/og/sqltablemodel.h>
#include <quickevent/og/itemdelegate.h>

#include <qf/qmlwidgets/dialogs/dialog.h>
#include <qf/qmlwidgets/framework/mainwindow.h>
#include <qf/qmlwidgets/dialogs/messagebox.h>
//#include <qf/qmlwidgets/framework/plugin.h>
#include <qf/qmlwidgets/toolbar.h>
#include <qf/qmlwidgets/combobox.h>

#include <qf/core/model/sqltablemodel.h>
#include <qf/core/sql/querybuilder.h>
#include <qf/core/sql/dbenum.h>
#include <qf/core/sql/connection.h>
#include <qf/core/sql/query.h>
#include <qf/core/sql/transaction.h>

#include <QDateTime>
#include <QLabel>
//#include <QMenu>
//#include <QSortFilterProxyModel>

#include <algorithm>

namespace qfs = qf::core::sql;
namespace qfw = qf::qmlwidgets;
namespace qff = qf::qmlwidgets::framework;
namespace qfd = qf::qmlwidgets::dialogs;
namespace qfm = qf::core::model;

RunsWidget::RunsWidget(QWidget *parent) :
	Super(parent),
	ui(new Ui::RunsWidget)
{
	ui->setupUi(this);

	ui->cbxDrawMethod->addItem(tr("Randomized equidistant clubs"), static_cast<int>(DrawMethod::RandomizedEquidistantClubs));
	ui->cbxDrawMethod->addItem(tr("Random number"), static_cast<int>(DrawMethod::RandomNumber));
	ui->cbxDrawMethod->addItem(tr("Equidistant clubs"), static_cast<int>(DrawMethod::EquidistantClubs));
	ui->cbxDrawMethod->addItem(tr("Stage 1 reverse order"), static_cast<int>(DrawMethod::StageReverseOrder));
	ui->cbxDrawMethod->addItem(tr("Handicap"), static_cast<int>(DrawMethod::Handicap));
	ui->frmDrawing->setVisible(false);

	QMetaObject::invokeMethod(this, "lazyInit", Qt::QueuedConnection);
}

RunsWidget::~RunsWidget()
{
	delete ui;
}

void RunsWidget::lazyInit()
{
}

void RunsWidget::reset(int class_id)
{
	if(eventPlugin()->eventName().isEmpty()) {
		ui->wRunsTableWidget->clear();
		return;
	}
	{
		m_cbxStage->blockSignals(true);
		m_cbxStage->clear();
		for(int i=0; i<eventPlugin()->stageCount(); i++)
			m_cbxStage->addItem(tr("E%1").arg(i+1), i+1);
		connect(m_cbxStage, SIGNAL(currentIndexChanged(int)), this, SLOT(reload()), Qt::UniqueConnection);
		connect(m_cbxStage, SIGNAL(currentIndexChanged(int)), this, SLOT(emitSelectedStageIdChanged(int)), Qt::UniqueConnection);
		m_cbxStage->blockSignals(false);
	}
	{
		m_cbxClasses->blockSignals(true);
		m_cbxClasses->loadItems(true);
		m_cbxClasses->insertItem(0, tr("--- all ---"), 0);
		if(class_id <= 0)
			m_cbxClasses->setCurrentIndex(1);
		else
			m_cbxClasses->setCurrentData(class_id);
		connect(m_cbxClasses, SIGNAL(currentDataChanged(QVariant)), this, SLOT(reload()), Qt::UniqueConnection);
		m_cbxClasses->blockSignals(false);
	}
	reload();
}

void RunsWidget::reload()
{
	qfLogFuncFrame();
	int stage_id = selectedStageId();
	int class_id = m_cbxClasses->currentData().toInt();
	ui->wRunsTableWidget->reload(stage_id, class_id);
}
/*
void RunsWidget::editStartList(int class_id, int competitor_id)
{
	reset(class_id);
	int stime_ix = m_runsModel->columnIndex("runs.startTimeMs");
	ui->tblRuns->horizontalHeader()->setSortIndicator(stime_ix, Qt::AscendingOrder);
	for (int i = 0; i < ui->tblRuns->model()->rowCount(); ++i) {
		auto r = ui->tblRuns->tableRow(i);
		if(r.value("competitorId").toInt() == competitor_id) {
			ui->tblRuns->setCurrentIndex(ui->tblRuns->model()->index(i, stime_ix));
		}
	}
}
*/
Event::EventPlugin *RunsWidget::eventPlugin()
{
	qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
	auto *plugin = qobject_cast<Event::EventPlugin *>(fwk->plugin("Event"));
	QF_ASSERT(plugin != nullptr, "Bad plugin", return nullptr);
	return plugin;
}

static Runs::RunsPlugin *runsPlugin()
{
	qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
	auto *plugin = qobject_cast<Runs::RunsPlugin *>(fwk->plugin("Runs"));
	QF_ASSERT(plugin != nullptr, "Bad plugin", return nullptr);
	return plugin;
}

void RunsWidget::settleDownInPartWidget(ThisPartWidget *part_widget)
{
	connect(part_widget, SIGNAL(resetPartRequest()), this, SLOT(reset()));
	connect(part_widget, SIGNAL(reloadPartRequest()), this, SLOT(reload()));
	/*
	qfw::Action *a = part_widget->menuBar()->actionForPath("station", true);
	a->setText("&Station");
	a->addActionInto(m_actCommOpen);
	*/
	qfw::ToolBar *main_tb = part_widget->toolBar("main", true);
	//main_tb->addAction(m_actCommOpen);
	{
		QLabel *lbl = new QLabel(tr("Stage "));
		main_tb->addWidget(lbl);
	}
	{
		m_cbxStage = new QComboBox();
		main_tb->addWidget(m_cbxStage);
	}
	{
		QLabel *lbl = new QLabel(tr("Class "));
		main_tb->addWidget(lbl);
	}
	{
		m_cbxClasses = new qfw::ForeignKeyComboBox();
		m_cbxClasses->setMaxVisibleItems(100);
		m_cbxClasses->setReferencedTable("classes");
		m_cbxClasses->setReferencedField("id");
		m_cbxClasses->setReferencedCaptionField("name");
		main_tb->addWidget(m_cbxClasses);
	}
	{
		auto *bt = new QPushButton(tr("&Draw options"));
		main_tb->addWidget(bt);
		bt->setCheckable(true);
		connect(bt, &QPushButton::toggled, ui->frmDrawing, &QFrame::setVisible);
	}
}

static bool list_length_greater_than(const QList<int> &lst1, const QList<int> &lst2)
{
	return lst1.count() > lst2.count();
}

static void shuffle(QList<int> &lst)
{
	std::random_shuffle(lst.begin(), lst.end());
}

QList< QList<int> > RunsWidget::runnersByClubSortedByCount(int stage_id, int class_id, QMap<int, QString> &runner_id_to_club)
{
	qfLogFuncFrame();
	QMap<QString, QList<int> > ids_by_clubs;
	qf::core::sql::QueryBuilder qb;
	qb.select2("runs", "id")
			.select2("competitors", "registration")
			.from("competitors")
			.joinRestricted("competitors.id", "runs.competitorId", "runs.stageId=" QF_IARG(stage_id))
			.where("competitors.classId=" QF_IARG(class_id));
	qfs::Query q;
	q.exec(qb.toString(), qf::core::Exception::Throw);
	while(q.next()) {
		QString club = q.value("registration").toString().mid(0, 3).trimmed().toUpper();
		int id = q.value("runs.id").toInt();
		ids_by_clubs[club] << id;
		runner_id_to_club[id] = club;
	}
	{
		for(auto club : ids_by_clubs.keys()) {
			QStringList sl;
			for(auto id : ids_by_clubs.value(club)) {
				sl << QString::number(id);
			}
			qfDebug() << "\t" << club << ":" << sl.join(", ");
		}
	}
	QList< QList<int> > ret = ids_by_clubs.values();
	// use list_length_greater_than to sort DESC
	qSort(ret.begin(), ret.end(), list_length_greater_than);
	for (int i = 0; i < ret.count(); ++i) {
		shuffle(ret[i]);
	}
	{
		for(auto lst : ret) {
			QStringList sl;
			for(auto id : lst) {
				sl << QString::number(id);
			}
			qfDebug() << "\t sorted:" << sl.join(", ");
		}
	}
	return ret;
}

QList<int> RunsWidget::runsForClass(int stage_id, int class_id)
{
	qfLogFuncFrame();
	QList<int> ret = competitorsForClass(stage_id, class_id).values();
	return ret;
}

QMap<int, int> RunsWidget::competitorsForClass(int stage_id, int class_id)
{
	qfLogFuncFrame();
	QMap<int, int> ret;
	qf::core::sql::QueryBuilder qb;
	qb.select2("runs", "id, competitorId")
			.from("competitors")
			.joinRestricted("competitors.id", "runs.competitorId", "NOT runs.offRace AND runs.stageId=" QF_IARG(stage_id), "JOIN")
			.where("competitors.classId=" QF_IARG(class_id));
	qfs::Query q;
	q.exec(qb.toString(), qf::core::Exception::Throw);
	while(q.next()) {
		ret[q.value("competitorId").toInt()] = q.value("id").toInt();
	}
	return ret;
}

void RunsWidget::on_btDraw_clicked()
{
	qfLogFuncFrame();
	int stage_id = eventPlugin()->currentStageId();
	QList<int> class_ids;
	int class_id = m_cbxClasses->currentData().toInt();
	if(class_id == 0) {
		if(!qf::qmlwidgets::dialogs::MessageBox::askYesNo(this, tr("Draw all clases without any start time set?"), false))
			return;
		qf::core::sql::QueryBuilder qb;
		qb.select2("competitors", "classId")
				.select("MIN(runs.startTimeMs) AS minStartTime")
				.select("MAX(runs.startTimeMs) AS maxStartTime")
				.from("competitors")
				.joinRestricted("competitors.id", "runs.competitorId", "runs.stageId=" QF_IARG(stage_id))
				.groupBy("competitors.classId")
				.as("t");
		qfs::Query q(qfs::Connection::forName());
		QString qs = "SELECT * FROM " + qb.toString() + " WHERE minStartTime IS NULL AND maxStartTime IS NULL";
		q.exec(qs, qf::core::Exception::Throw);
		while(q.next())
			class_ids << q.value("classId").toInt();
	}
	else {
		class_ids << class_id;
	}
	try {
		qf::core::sql::Transaction transaction(qfs::Connection::forName());
		for(int class_id : class_ids) {
			int handicap_length_ms = eventPlugin()->eventConfig()->handicapLength() * 60 * 1000;
			QVector<int> handicap_times;
			qf::core::sql::QueryBuilder qb;
			qb.select2("classdefs", "startTimeMin, startIntervalMin, vacantsBefore, vacantEvery, vacantsAfter")
					.from("classdefs")
					.where("stageId=" QF_IARG(stage_id))
					.where("classId=" QF_IARG(class_id));
			qfs::Query q_classdefs(transaction.connection());
			q_classdefs.exec(qb.toString(), qf::core::Exception::Throw);
			if(!q_classdefs.next())
				continue;
			QList<int> runners_draw_ids;
			DrawMethod draw_method = DrawMethod(ui->cbxDrawMethod->currentData().toInt());
			qfDebug() << "DrawMethod:" << (int)draw_method;
			if(draw_method == DrawMethod::RandomNumber) {
				runners_draw_ids = runsForClass(stage_id, class_id);
				shuffle(runners_draw_ids);
			}
			else if(draw_method == DrawMethod::Handicap) {
				int stage_count = eventPlugin()->eventConfig()->stageCount();
				qf::core::utils::Table results = runsPlugin()->nstagesResultsTable(stage_count - 1, class_id);
				QMap<int, int> competitor_to_run = competitorsForClass(stage_count, class_id);
				//int n = 0;
				for (int i = 0; i < results.rowCount(); ++i) {
					qf::core::utils::TableRow r = results.row(i);
					int run_id = competitor_to_run.take(r.value("competitors.id").toInt());
					if(run_id > 0) {
						runners_draw_ids << run_id;
						int loss_ms = r.value("timeLossMs").toInt();
						if(loss_ms < handicap_length_ms) {
							qfDebug() << "loss:" << loss_ms;
							handicap_times << loss_ms;
						}
					}
				}
				runners_draw_ids << competitor_to_run.values();
			}
			else if(draw_method == DrawMethod::StageReverseOrder) {
				QMap<int, int> competitor_to_run = competitorsForClass(stage_id, class_id);
				qf::core::sql::QueryBuilder qb1;
				qb1.select2("runs", "competitorId")
						//.select2("competitors", "lastName")
						.from("competitors")
						.joinRestricted("competitors.id", "runs.competitorId", "runs.stageId=" QF_IARG(1), "JOIN")
						.where("competitors.classId=" QF_IARG(class_id))
						.orderBy("runs.startTimeMs DESC");
				qfs::Query q(transaction.connection());
				q.exec(qb1.toString(), qf::core::Exception::Throw);
				while(q.next()) {
					int competitor_id = q.value("competitorId").toInt();
					if(competitor_to_run.contains(competitor_id)) {
						//qfDebug() << "\t adding to poll";
						runners_draw_ids << competitor_to_run.take(competitor_id);
					}
				}
				runners_draw_ids << competitor_to_run.values();
			}
			else if(draw_method == DrawMethod::EquidistantClubs || draw_method == DrawMethod::RandomizedEquidistantClubs) {
				QMap<int, QString> runner_id_to_club;
				QList< QList<int> > runners_by_club = runnersByClubSortedByCount(stage_id, class_id, runner_id_to_club);
				if(runners_by_club.count()) {
					int runners_cnt = 0;
					for(auto lst : runners_by_club)
						runners_cnt += lst.count();
					runners_draw_ids.reserve(runners_cnt);
					runners_draw_ids << runners_by_club.first();
					int club_ix = 1;
					int slot_len = 1;
					int insert_ix = slot_len;
					while(club_ix < runners_by_club.count()) {
						QList<int> &club_runners = runners_by_club[club_ix];
						QF_ASSERT(club_runners.count() > 0, "Club runners cannot be empty in this phase!", break);
						int runner_id = club_runners.takeFirst();
						qfDebug() << "inserting id:" << runner_id << "at index:" << insert_ix;
						runners_draw_ids.insert(insert_ix, runner_id);

						// set club index for next scan
						if(club_runners.isEmpty()) {
							// take next club
							club_ix++;
							qfDebug() << "club_ix:" << club_ix;
						}

						// set insert index for next scan
						insert_ix += slot_len + 1;
						if(insert_ix > runners_draw_ids.count()) {
							slot_len++;
							insert_ix = slot_len;
						}
					}
				}
				if(draw_method == DrawMethod::RandomizedEquidistantClubs) {
					qsrand(QTime::currentTime().msecsSinceStartOfDay());
					int cnt = runners_draw_ids.count();
					for (int i = 0; i < 2*cnt; ++i) {
						// randomly switch rudders fi their clubs will not get consequent
						int ix1 = (int)(qrand() * (double)cnt / RAND_MAX);
						if(ix1 >= cnt)
							ix1 = cnt - 1;
						int ix2 = (int)(qrand() * (double)cnt / RAND_MAX);
						if(ix2 >= cnt)
							ix2 = cnt - 1;
						if(ix1 == ix2)
							continue;
						if((ix1 - ix2) == 1 || (ix2 - ix1) == 1)
							continue;
						QString club1 = runner_id_to_club.value(runners_draw_ids[ix1]);
						QString club2 = runner_id_to_club.value(runners_draw_ids[ix2]);
						if(ix1 > 0) {
							QString club = runner_id_to_club.value(runners_draw_ids[ix1 - 1]);
							if(club == club2)
								continue;
						}
						if(ix1 < cnt - 1) {
							QString club = runner_id_to_club.value(runners_draw_ids[ix1 + 1]);
							if(club == club2)
								continue;
						}
						if(ix2 > 0) {
							QString club = runner_id_to_club.value(runners_draw_ids[ix2 - 1]);
							if(club == club1)
								continue;
						}
						if(ix2 < cnt - 1) {
							QString club = runner_id_to_club.value(runners_draw_ids[ix2 + 1]);
							if(club == club1)
								continue;
						}
						// can switch
						int id = runners_draw_ids[ix1];
						runners_draw_ids[ix1] = runners_draw_ids[ix2];
						runners_draw_ids[ix2] = id;
					}
				}
			}
			if(runners_draw_ids.count()) {
				// save drawing to SQL
				int interval = q_classdefs.value("startIntervalMin").toInt() * 60 * 1000;
				if(interval == 0) {
					if(!qf::qmlwidgets::dialogs::MessageBox::askYesNo(this, tr("Start interval is zero, proceed anyway?"), false))
						continue;
				}
				int start0 = q_classdefs.value("startTimeMin").toInt() * 60 * 1000;
				int vacants_before = q_classdefs.value("vacantsBefore").toInt();
				int vacant_every = q_classdefs.value("vacantEvery").toInt();
				int start = start0;
				int n = 0;

				if(draw_method != DrawMethod::Handicap) {
					start += vacants_before * interval;
				}

				qfs::Query q(transaction.connection());
				q.prepare("UPDATE runs SET startTimeMs=:startTimeMs WHERE id=:id");
				for(int runner_id : runners_draw_ids) {
					q.bindValue(QStringLiteral(":id"), runner_id);
					if(draw_method == DrawMethod::Handicap) {
						if(handicap_times.isEmpty()) {
							++n;
							start = start0 + handicap_length_ms + n * interval;
						}
						else {
							start = start0 + handicap_times.takeFirst();
						}
						q.bindValue(QStringLiteral(":startTimeMs"), start);
						q.exec(qf::core::Exception::Throw);
					}
					else {
						q.bindValue(QStringLiteral(":startTimeMs"), start);
						q.exec(qf::core::Exception::Throw);
						start += interval;
						if(n > 0 && vacant_every > 0 && (n % vacant_every) == 0)
							start += interval;
						n++;
					}
				}
			}
		}
		transaction.commit();
	}
	catch (const qf::core::Exception &e) {
		qf::qmlwidgets::dialogs::MessageBox::showException(this, e);
	}
	ui->wRunsTableWidget->runsModel()->reload();
	//reload();
}

void RunsWidget::on_btDrawRemove_clicked()
{
	if(!qf::qmlwidgets::dialogs::MessageBox::askYesNo(this, tr("Reset all start times for this class?"), false))
		return;
	try {
		auto *runs_model = ui->wRunsTableWidget->runsModel();
		qf::core::sql::Transaction transaction(qfs::Connection::forName());
		for (int i = 0; i < runs_model->rowCount(); ++i) {
			runs_model->setValue(i, "startTimeMs", QVariant());
			// bypass mid-air collision check
			runs_model->quickevent::og::SqlTableModel::postRow(i, qf::core::Exception::Throw);
		}
		transaction.commit();
		runs_model->reload();
	}
	catch (const qf::core::Exception &e) {
		qf::qmlwidgets::dialogs::MessageBox::showException(this, e);
	}
}

void RunsWidget::emitSelectedStageIdChanged(int ix)
{
	emit selectedStageIdChanged(ix + 1);
}

int RunsWidget::selectedStageId()
{
	return m_cbxStage->currentData().toInt();
}

