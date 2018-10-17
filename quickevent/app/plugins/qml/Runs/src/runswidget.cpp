#include "runswidget.h"
#include "ui_runswidget.h"
#include "thispartwidget.h"
#include "runstablemodel.h"
#include "runstableitemdelegate.h"
#include "Runs/runsplugin.h"

#include <Event/eventplugin.h>

#include <quickevent/core/og/sqltablemodel.h>
#include <quickevent/gui/og/itemdelegate.h>

#include <qf/qmlwidgets/dialogs/dialog.h>
#include <qf/qmlwidgets/framework/mainwindow.h>
#include <qf/qmlwidgets/dialogs/messagebox.h>
#include <qf/qmlwidgets/dialogs/filedialog.h>
#include <qf/qmlwidgets/menubar.h>
#include <qf/qmlwidgets/action.h>
#include <qf/qmlwidgets/toolbar.h>
#include <qf/qmlwidgets/combobox.h>

#include <qf/core/model/sqltablemodel.h>
#include <qf/core/sql/querybuilder.h>
#include <qf/core/sql/dbenum.h>
#include <qf/core/sql/connection.h>
#include <qf/core/sql/query.h>
#include <qf/core/sql/transaction.h>
#include <qf/core/assert.h>

#include <QCheckBox>
#include <QDateTime>
#include <QLabel>
#include <QInputDialog>

#include <algorithm>

namespace qfs = qf::core::sql;
namespace qfw = qf::qmlwidgets;
namespace qff = qf::qmlwidgets::framework;
namespace qfd = qf::qmlwidgets::dialogs;
namespace qfm = qf::core::model;

static Event::EventPlugin *eventPlugin()
{
	qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
	auto *plugin = qobject_cast<Event::EventPlugin *>(fwk->plugin("Event"));
	QF_ASSERT_EX(plugin != nullptr, "Bad plugin");
	return plugin;
}

static Runs::RunsPlugin *runsPlugin()
{
	qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
	auto *plugin = qobject_cast<Runs::RunsPlugin *>(fwk->plugin("Runs"));
	QF_ASSERT_EX(plugin != nullptr, "Bad plugin");
	return plugin;
}

RunsWidget::RunsWidget(QWidget *parent) :
	Super(parent),
	ui(new Ui::RunsWidget)
{
	ui->setupUi(this);

	//bool is_relays = eventPlugin()->eventConfig()->isRelays();

	ui->frmDrawing->setVisible(false);

	connect(eventPlugin(), &Event::EventPlugin::eventOpened, [this](QString event_name) {
		ui->cbxDrawMethod->clear();
		if(event_name.isEmpty())
			return;
		bool is_relays = eventPlugin()->eventConfig()->isRelays();
		if(is_relays) {
			ui->cbxDrawMethod->addItem(tr("Relays first leg"), static_cast<int>(DrawMethod::Relays));
		}
		else {
			ui->cbxDrawMethod->addItem(tr("Randomized equidistant clubs"), static_cast<int>(DrawMethod::RandomizedEquidistantClubs));
			ui->cbxDrawMethod->addItem(tr("Random number"), static_cast<int>(DrawMethod::RandomNumber));
			ui->cbxDrawMethod->addItem(tr("Equidistant clubs"), static_cast<int>(DrawMethod::EquidistantClubs));
			ui->cbxDrawMethod->addItem(tr("Stage 1 reverse order"), static_cast<int>(DrawMethod::StageReverseOrder));
			ui->cbxDrawMethod->addItem(tr("Handicap"), static_cast<int>(DrawMethod::Handicap));
			ui->cbxDrawMethod->addItem(tr("Keep runners order"), static_cast<int>(DrawMethod::KeepOrder));
			ui->cbxDrawMethod->addItem(tr("Grouped: C, B+A (PSOB DH12-14)"), static_cast<int>(DrawMethod::GroupedC));
			ui->cbxDrawMethod->addItem(tr("Grouped: C, B, A+E+R (PSOB DH16-20)"), static_cast<int>(DrawMethod::GroupedCB));
			ui->cbxDrawMethod->addItem(tr("Grouped by ranking (PSOB DH21L)"), static_cast<int>(DrawMethod::GroupedRanking));
		}
	});

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
	bool is_relays = eventPlugin()->eventConfig()->isRelays();
	if(!eventPlugin()->isEventOpen()) {
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
	if(is_relays) {
		connect(m_cbxLeg, SIGNAL(currentIndexChanged(int)), this, SLOT(reload()), Qt::UniqueConnection);
	}
	else {
		//m_cbxLeg->setVisible(false);
		//m_lblLegs->setVisible(false);
		QF_SAFE_DELETE(m_lblLegs);
		QF_SAFE_DELETE(m_cbxLeg);
	}
	//qfWarning() << "is relays:" << is_relays << "legs visible:" << m_cbxLeg->isVisible();
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
	bool is_relays = eventPlugin()->eventConfig()->isRelays();
	int stage_id = is_relays? m_cbxLeg->currentData().toInt(): selectedStageId();
	int class_id = m_cbxClasses->currentData().toInt();
	ui->wRunsTableWidget->reload(stage_id, class_id, m_chkShowOffRace->isChecked());
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
void RunsWidget::settleDownInPartWidget(ThisPartWidget *part_widget)
{
	connect(part_widget, SIGNAL(resetPartRequest()), this, SLOT(reset()));
	connect(part_widget, SIGNAL(reloadPartRequest()), this, SLOT(reload()));

	qfw::Action *a_print = part_widget->menuBar()->actionForPath("print", true);
	a_print->setText("&Print");

	qfw::Action *a_import = part_widget->menuBar()->actionForPath("import", true);
	a_import->setText("&Import");

	qfw::Action *a_export = part_widget->menuBar()->actionForPath("export", true);
	a_export->setText("E&xport");

	qfw::Action *a_import_start_times = a_import->addMenuInto("startTimes", tr("Start times"));
	qfw::Action *a_import_start_times_ob2000 = new qfw::Action("ob2000", tr("OB 2000"));
	a_import_start_times->addActionInto(a_import_start_times_ob2000);
	connect(a_import_start_times_ob2000, &qfw::Action::triggered, this, &RunsWidget::import_start_times_ob2000);


	qfw::Action *a_export_results = a_export->addMenuInto("results", tr("Results"));
	qfw::Action *a_export_results_csos = a_export_results->addMenuInto("csos", tr("CSOS"));
	qfw::Action *a_export_results_csos_stage = a_export_results_csos->addActionInto("stage", tr("Current stage"));
	connect(a_export_results_csos_stage, &qfw::Action::triggered, this, &RunsWidget::export_results_csos_stage);
	qfw::Action *a_export_results_csos_overall = a_export_results_csos->addActionInto("overall", tr("Overall"));
	connect(a_export_results_csos_overall, &qfw::Action::triggered, this, &RunsWidget::export_results_csos_overall);

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
	QLabel *lbl_classes;
	{
		lbl_classes = new QLabel(tr("&Class "));
		main_tb->addWidget(lbl_classes);
	}
	{
		m_cbxClasses = new qfw::ForeignKeyComboBox();
		m_cbxClasses->setMaxVisibleItems(100);
		m_cbxClasses->setReferencedTable("classes");
		m_cbxClasses->setReferencedField("id");
		m_cbxClasses->setReferencedCaptionField("name");
		main_tb->addWidget(m_cbxClasses);
	}
	lbl_classes->setBuddy(m_cbxClasses);
	{
		{
			m_lblLegs = new QLabel(tr("&Leg "));
			main_tb->addWidget(m_lblLegs);
		}
		{
			m_cbxLeg = new QComboBox();
			m_cbxLeg->addItem(tr("--- all ---"), 0);
			m_cbxLeg->addItem("1", 1);
			m_cbxLeg->addItem("2", 2);
			m_cbxLeg->addItem("3", 3);
			m_cbxLeg->addItem("4", 4);
			m_cbxLeg->addItem("5", 5);
			m_cbxLeg->addItem("6", 6);
			m_cbxLeg->addItem("7", 7);
			m_cbxLeg->addItem("8", 8);
			m_cbxLeg->addItem("9", 9);
			m_cbxLeg->addItem("10", 10);
			main_tb->addWidget(m_cbxLeg);
		}
		m_lblLegs->setBuddy(m_cbxClasses);
	}

	{
		m_chkShowOffRace = new QCheckBox();
		m_chkShowOffRace->setText(tr("Show o&ff-race"));
		m_chkShowOffRace->setToolTip(tr("Show off race competitors"));
		connect(m_chkShowOffRace, &QCheckBox::toggled, this, &RunsWidget::reload);
		main_tb->addWidget(m_chkShowOffRace);
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

QList<int> RunsWidget::runsForClass(int stage_id, int class_id, const QString &extra_where_condition, const QString &order_by)
{
	qfLogFuncFrame();
	QList<int> ret = competitorsForClass(stage_id, class_id, extra_where_condition, order_by).values();
	return ret;
}

QMap<int, int> RunsWidget::competitorsForClass(int stage_id, int class_id, const QString &extra_where_condition, const QString &order_by)
{
	qfLogFuncFrame() << "stage:" << stage_id << "class:" << class_id;
	bool is_relays = eventPlugin()->eventConfig()->isRelays();
	QMap<int, int> ret;
	qf::core::sql::QueryBuilder qb;
	qb.select2("runs", "id, competitorId")
			.from("competitors")
			.joinRestricted("competitors.id", "runs.competitorId", "runs.isRunning AND runs.stageId=" QF_IARG(stage_id), "JOIN");
	if(is_relays) {
		qb.joinRestricted("runs.relayId", "relays.id", "relays.classId=" QF_IARG(class_id), "JOIN");
	}
	else {
		qb.where("competitors.classId=" QF_IARG(class_id));
	}
	if(!extra_where_condition.isEmpty())
		qb.where(extra_where_condition);
	if(!order_by.isEmpty())
		qb.orderBy(order_by);
	qfs::Query q;
	q.exec(qb.toString(), qf::core::Exception::Throw);
	while(q.next()) {
		ret[q.value("competitorId").toInt()] = q.value("id").toInt();
	}
	return ret;
}

void RunsWidget::import_start_times_ob2000()
{
	qfLogFuncFrame();
	QString fn = qf::qmlwidgets::dialogs::FileDialog::getOpenFileName(this, tr("Import"));
	if(!fn.isEmpty()) {
		QFile f(fn);
		if(f.open(QFile::ReadOnly)) {
			struct Run {
				int runsId;
				QString className;
				//int startTimeMs;
			};
			try {
				qf::core::sql::Transaction transaction;
				qfs::Query q;
				QMap< QString, Run > runners;
				{
					qf::core::sql::QueryBuilder qb;
					qb.select2("runs", "id, startTimeMs")
							.select2("competitors", "registration")
							.select2("classes", "name")
							.from("competitors")
							.joinRestricted("competitors.id", "runs.competitorId", "runs.isRunning AND runs.stageId=" QF_IARG(selectedStageId()), "JOIN")
							.join("competitors.classId", "classes.id");
					q.exec(qb.toString(), qf::core::Exception::Throw);
					while(q.next()) {
						QString reg = q.value("registration").toString().toUpper().trimmed();
						if(reg.isEmpty())
							continue;
						auto &runner = runners[reg];
						runner.runsId = q.value("runs.id").toInt();
						runner.className = q.value("classes.name").toString();
					}
				}
				q.prepare("UPDATE runs SET startTimeMs=:startTimeMs WHERE id=:id");
				while(!f.atEnd()) {
					QByteArray ba = f.readLine();
					if(!ba.startsWith("    "))
						continue;
					qfDebug() << ba.size() << ba;
					//if(ba.isEmpty())
					//	break;
					QStringList sl = QString::fromLatin1(ba).split(' ', QString::SkipEmptyParts);
					if(sl.count() < 5)
						continue;
					bool ok;
					double d = sl.value(0).toDouble(&ok);
					if(!ok) {
						qfWarning() << "Cannot convert" << sl.value(0) << "to double.";
						qfInfo() << ba;
						continue;
					}
					int st_time = ((int)d) * 60 + (((int)(d * 100)) % 100);
					st_time *= 1000;
					QString reg = sl.value(sl.count() - 2) + sl.value(sl.count() - 1);
					reg = reg.toUpper().trimmed();
					QString class_name = sl.value(sl.count() - 3);
					if(runners.contains(reg)) {
						const auto &runner = runners[reg];
						if(runner.className == class_name) {
							q.bindValue(QStringLiteral(":id"), runner.runsId);
							q.bindValue(QStringLiteral(":startTimeMs"), st_time);
							q.exec(qf::core::Exception::Throw);
							qfDebug() << reg << "->" << QString::number(st_time / 1000 / 60) + '.' + QString::number(st_time / 1000 % 60);
						}
						else {
							qfWarning() << "Different classname" << class_name << runner.className << "start time will not be imported";
							qfInfo() << ba;
						}
					}
				}
				transaction.commit();
			}
			catch (const qf::core::Exception &e) {
				qf::qmlwidgets::dialogs::MessageBox::showException(this, e);
			}
			ui->wRunsTableWidget->runsModel()->reload();
		}
	}
}

static QString make_width(const QString &s, int width)
{
	static const auto SS = QStringLiteral("%1");
	int abs_width = width < 0? -width: width;
	QString ret = SS.arg(s, width, QChar(' ')).mid(0, abs_width);
	return ret;
}

QString RunsWidget::getSaveFileName(const QString &file_name, int stage_id)
{
	QString fn = file_name;
	QString ext;
	int ix = fn.lastIndexOf('.');
	if(ix > 0)
		ext = fn.mid(ix);
	Event::EventPlugin *evp = eventPlugin();
	if(evp->stageCount() > 1 && stage_id > 0)
		fn = QStringLiteral("e%1-").arg(stage_id) + fn;;

	fn = qfd::FileDialog::getSaveFileName(this, tr("Save as %1").arg(ext.mid(1).toUpper()), fn, '*' + ext);
	if(!fn.isEmpty()) {
		if(!fn.endsWith(ext, Qt::CaseInsensitive))
			fn += ext;
	}
	return fn;
}

void RunsWidget::writeCSOSHeader(QTextStream &ts)
{
	Event::EventPlugin *evp = eventPlugin();
	int stage_id = selectedStageId();
	QDateTime start_dt = evp->stageStartDateTime(stage_id);
	Event::EventConfig *ec = evp->eventConfig();
	static constexpr int HWIDTH = -19;
	ts << make_width("Kod zavodu", HWIDTH) << ": " << ec->importId() << "\r\n";
	ts << make_width("Nazev zavodu", HWIDTH) << ": " << ec->eventName() << "\r\n";
	ts << make_width("Zarazeni do soutezi", HWIDTH) << ": " << "" << "\r\n";
	ts << make_width("Datum konani", HWIDTH) << ": " << start_dt.toString(Qt::ISODate) << "\r\n";
	ts << make_width("Misto konani", HWIDTH) << ": " << ec->eventPlace() << "\r\n";
	ts << make_width("Poradatel", HWIDTH) << ": " << "" << "\r\n";
	ts << make_width("Mapa", HWIDTH) << ": " << "" << "\r\n";
	ts << make_width("Reditel zavodu", HWIDTH) << ": " << ec->director() << "\r\n";
	ts << make_width("Hlavni rozhodci", HWIDTH) << ": " << ec->mainReferee() << "\r\n";
	ts << make_width("Stavitel trati", HWIDTH) << ": " << "" << "\r\n";
	ts << make_width("JURY", HWIDTH) << ": " << "" << "\r\n";
	ts << make_width("Protokol", HWIDTH) << ": " << "\r\n";
	ts << "----------------------------------------------------------------------------\r\n";
}

void RunsWidget::export_results_csos_stage()
{
	int stage_id = selectedStageId();
	QString fn = getSaveFileName("results-csos.txt", stage_id);
	if(fn.isEmpty())
		return;

	QFile f(fn);
	if(!f.open(QIODevice::WriteOnly)) {
		qfError() << "Cannot open file" << f.fileName() << "for writing.";
		return;
	}
	QTextStream ts(&f);
	ts.setCodec("CP1250");
	writeCSOSHeader(ts);

	/*
	Runs::RunsPlugin *rp = runsPlugin();
	qf::core::utils::Table tt = rp->currentStageResultsTable();
	for (int i = 0; i < tt.rowCount(); ++i) {
		qf::core::utils::TableRow row = tt.row(i);
	}
	*/
	qfs::QueryBuilder qb;
	qb.select2("competitors", "registration, lastName, firstName, licence")
		.select("COALESCE(competitors.lastName, '') || ' ' || COALESCE(competitors.firstName, '') AS competitorName")
		.select2("runs", "*")
		.select2("classes", "name")
		.from("competitors")
		.join("competitors.classId", "classes.id")
		.joinRestricted("competitors.id"
						, "runs.competitorId"
						, QStringLiteral("runs.stageId=%1 AND runs.isRunning AND runs.finishTimeMs>0").arg(stage_id)
						, qfs::QueryBuilder::INNER_JOIN)
		//.where("competitors.classId={{class_id}}")
		.orderBy("classes.name, runs.notCompeting, runs.disqualified, runs.timeMs");
	qfs::Query q;
	q.exec(qb.toString());
	while(q.next()) {
		ts << make_width(q.value(QStringLiteral("classes.name")).toString(), -10);
		ts << make_width(q.value(QStringLiteral("competitorName")).toString(), -25);
		ts << make_width(q.value(QStringLiteral("registration")).toString(), -7);
		ts << (q.value(QStringLiteral("runs.notCompeting")).toBool()? "M": make_width(q.value(QStringLiteral("licence")).toString(), 1));
		int ms = q.value(QStringLiteral("runs.timeMs")).toInt();
		QString time_str;
		if(q.value(QStringLiteral("runs.disqualified")).toBool())
			time_str = QStringLiteral("888.88");
		/*
		else if(PRUMER neni podporovan v QE)
			time_str = QStringLiteral("999.99");
		*/
		else
			time_str = make_width(quickevent::core::og::TimeMs(ms).toString(), 6);
		ts << time_str;
		ts << "\r\n";
	}
}

void RunsWidget::export_results_csos_overall()
{
	QString fn = getSaveFileName("overall-results-csos.txt", 0);
	if(fn.isEmpty())
		return;

	QFile f(fn);
	if(!f.open(QIODevice::WriteOnly)) {
		qfError() << "Cannot open file" << f.fileName() << "for writing.";
		return;
	}
	QTextStream ts(&f);
	ts.setCodec("CP1250");
	writeCSOSHeader(ts);

	Event::EventPlugin *evp = eventPlugin();
	int stage_count = evp->stageCount();

	Runs::RunsPlugin *rp = runsPlugin();

	qfs::Query q;
	q.exec("SELECT id, name FROM classes ORDER BY name");
	while(q.next()) {
		int class_id = q.value(0).toInt();
		QString class_name = q.value(1).toString();
		qf::core::utils::Table tt = rp->nstagesResultsTable(stage_count, class_id, -1, false);
		for (int i = 0; i < tt.rowCount(); ++i) {
			qf::core::utils::TableRow row = tt.row(i);
			ts << make_width(class_name, -10);
			ts << make_width(row.value(QStringLiteral("competitorName")).toString(), -25);
			ts << make_width(row.value(QStringLiteral("registration")).toString(), -7);
			ts << (row.value(QStringLiteral("pos")).toString() == QLatin1String("N")? "M": make_width(row.value(QStringLiteral("licence")).toString(), 1));
			int ms = row.value(QStringLiteral("timeMs")).toInt();
			QString time_str;
			if(row.value(QStringLiteral("pos")).toString() == QLatin1String("D"))
				time_str = QStringLiteral("888.88");
			/*
			else if(PRUMER neni podporovan v QE)
				time_str = QStringLiteral("999.99");
			*/
			else
				time_str = make_width(quickevent::core::og::TimeMs(ms).toString(), 6);
			ts << time_str;
			ts << "\r\n";
		}
	}
}

bool RunsWidget::isLockedForDrawing(int class_id, int stage_id)
{
	qf::core::sql::QueryBuilder qb;
	qb.select2("classdefs", "drawLock")
			.select2("classes", "name")
			.from("classdefs")
			.join("classdefs.classId", "classes.id")
			.where("stageId=" QF_IARG(stage_id))
			.where("classId=" QF_IARG(class_id) );
	qfs::Query q;
	QString qs = qb.toString();
	q.exec(qs, qf::core::Exception::Throw);
	if(q.next()) {
		bool is_locked = q.value("drawLock").toBool();
		return is_locked;
	}
	return false;
}

void RunsWidget::saveLockedForDrawing(int class_id, int stage_id, bool is_locked, int start_last_min)
{
	qfs::Query q;
	q.prepare("UPDATE classdefs SET drawLock=:drawLock, lastStartTimeMin=:lastStartTimeMin"
			  " WHERE stageId=" QF_IARG(stage_id) " AND classId=" QF_IARG(class_id), qf::core::Exception::Throw);
	q.bindValue(":drawLock", is_locked);
	q.bindValue(":lastStartTimeMin", start_last_min);
	q.exec(qf::core::Exception::Throw);
}

void RunsWidget::on_btDraw_clicked()
{
	qfLogFuncFrame();
	bool is_relays = eventPlugin()->eventConfig()->isRelays();
	int stage_id = selectedStageId();
	DrawMethod draw_method = DrawMethod(ui->cbxDrawMethod->currentData().toInt());
	Event::StageData stage_data = eventPlugin()->stageData(stage_id);
	bool use_all_maps = stage_data.isUseAllMaps();
	qfDebug() << "DrawMethod:" << (int)draw_method << "use_all_maps:" << use_all_maps;
	QList<int> class_ids;
	int class_id = m_cbxClasses->currentData().toInt();
	if(class_id == 0) {
		if(!qf::qmlwidgets::dialogs::MessageBox::askYesNo(this, tr("Draw all clases without draw lock?"), false))
			return;
		qf::core::sql::QueryBuilder qb;
		qb.select2("classdefs", "classId")
				.from("classdefs")
				.where("stageId=" QF_IARG(stage_id))
				.where("NOT drawLock");
		qfs::Query q;
		QString qs = qb.toString();
		q.exec(qs, qf::core::Exception::Throw);
		while(q.next())
			class_ids << q.value("classId").toInt();
	}
	else {
		bool is_locked = isLockedForDrawing(class_id, stage_id);
		if(is_locked) {
			qf::qmlwidgets::dialogs::MessageBox::showInfo(this, tr("Class is locked for drawing."));
			return;
		}
		class_ids << class_id;
	}

	try {
		qf::core::sql::Transaction transaction;
		for(int class_id : class_ids) {
			int handicap_length_ms = eventPlugin()->eventConfig()->handicapLength() * 60 * 1000;
			QVector<int> handicap_times;
			qf::core::sql::QueryBuilder qb;
			qb.select2("classdefs", "startTimeMin, startIntervalMin, vacantsBefore, vacantEvery, vacantsAfter, mapCount")
					.from("classdefs")
					.where("stageId=" QF_IARG(stage_id))
					.where("classId=" QF_IARG(class_id));
			qfs::Query q_classdefs(transaction.connection());
			q_classdefs.exec(qb.toString(), qf::core::Exception::Throw);
			if(!q_classdefs.next())
				continue;
			QList<int> runners_draw_ids;
			if(draw_method == DrawMethod::RandomNumber) {
				runners_draw_ids = runsForClass(stage_id, class_id);
				shuffle(runners_draw_ids);
			}
			else if(draw_method == DrawMethod::KeepOrder) {
				runners_draw_ids = runsForClass(stage_id, class_id, QString(), "runs.startTimeMs");
			}
			else if(draw_method == DrawMethod::Relays) {
				runners_draw_ids = runsForClass(stage_id, class_id, "runs.leg=1");
			}
			else if(draw_method == DrawMethod::GroupedC) {
				QList<int> group1 = runsForClass(stage_id, class_id, "licence='C' or licence is null");
				QList<int> group2 = runsForClass(stage_id, class_id, "licence='A' or licence='B'");
				shuffle(group1);
				shuffle(group2);
				runners_draw_ids = group1 + group2;
			}
			else if(draw_method == DrawMethod::GroupedCB) {
				QList<int> group1 = runsForClass(stage_id, class_id, "licence='C' or licence is null");
				QList<int> group2 = runsForClass(stage_id, class_id, "licence='B'");
				QList<int> group3 = runsForClass(stage_id, class_id, "licence='A' or licence='R' or licence='E'");
				shuffle(group1);
				shuffle(group2);
				shuffle(group3);
				runners_draw_ids = group1 + group2 + group3;
			}
			else if(draw_method == DrawMethod::GroupedRanking) {
				QList<int> group1 = runsForClass(stage_id, class_id, "ranking>300 or ranking is null");
				QList<int> group2 = runsForClass(stage_id, class_id, "ranking>100 and ranking<=300");
				QList<int> group3 = runsForClass(stage_id, class_id, "ranking<=100");
				shuffle(group1);
				shuffle(group2);
				shuffle(group3);
				runners_draw_ids = group1 + group2 + group3;
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
				if(interval == 0 && !is_relays) {
					if(!qf::qmlwidgets::dialogs::MessageBox::askYesNo(this, tr("Start interval is zero, proceed anyway?"), false))
						continue;
				}
				int start0 = q_classdefs.value("startTimeMin").toInt() * 60 * 1000;
				int vacants_before = q_classdefs.value("vacantsBefore").toInt();
				int vacant_every = q_classdefs.value("vacantEvery").toInt();
				int vacants_after = q_classdefs.value("vacantsAfter").toInt();
				int map_count = q_classdefs.value("mapCount").toInt();
				int start = start0;
				int n = 0;

				if(map_count <= 0)
					use_all_maps = false;

				if(draw_method != DrawMethod::Handicap) {
					start += vacants_before * interval;
					if(use_all_maps) {
						map_count -= vacants_before;
						int spare_map_count = (map_count - vacants_after - runners_draw_ids.count());
						if(spare_map_count > 0)
							vacant_every = runners_draw_ids.count() / spare_map_count;
						else
							vacant_every = 0;
					}
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
						n++;
						map_count--;
						bool can_add_vacant = true;
						if(use_all_maps) {
							can_add_vacant = (map_count > vacants_after);
						}
						if(can_add_vacant && vacant_every > 0 && (n % vacant_every) == 0)
							start += interval;
					}
				}
				if(use_all_maps)
					vacants_after = map_count;
				if(vacants_after > 0)
					start += (vacants_after - 1) * interval;
				saveLockedForDrawing(class_id, stage_id, true, start / 60 / 1000);
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
	int class_id = m_cbxClasses->currentData().toInt();
	if(class_id == 0)
		return;
	if(!qf::qmlwidgets::dialogs::MessageBox::askYesNo(this, tr("Reset all start times and unlock for drawing for this class?"), false))
		return;
	try {
		auto *runs_model = ui->wRunsTableWidget->runsModel();
		qf::core::sql::Transaction transaction(qfs::Connection::forName());
		for (int i = 0; i < runs_model->rowCount(); ++i) {
			runs_model->setValue(i, "startTimeMs", QVariant());
			// bypass mid-air collision check
			runs_model->quickevent::core::og::SqlTableModel::postRow(i, qf::core::Exception::Throw);
		}
		int stage_id = selectedStageId();
		saveLockedForDrawing(class_id, stage_id, false, 0);
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

