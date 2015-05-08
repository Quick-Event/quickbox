#include "runswidget.h"
#include "ui_runswidget.h"
#include "thispartwidget.h"
#include "runstablemodel.h"
#include "runstableitemdelegate.h"

#include <Event/eventplugin.h>

#include <quickevent/og/timems.h>
#include <quickevent/og/sqltablemodel.h>
#include <quickevent/og/itemdelegate.h>

#include <qf/qmlwidgets/dialogs/dialog.h>
#include <qf/qmlwidgets/dialogs/messagebox.h>
#include <qf/qmlwidgets/framework/mainwindow.h>
#include <qf/qmlwidgets/framework/plugin.h>
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

	ui->cbxDrawMethod->addItem(tr("Random number"), static_cast<int>(DrawMethod::RandomNumber));
	ui->frmDrawing->setVisible(false);

	ui->tblRunsToolBar->setTableView(ui->tblRuns);

	ui->tblRuns->setPersistentSettingsId("tblRuns");
	ui->tblRuns->setRowEditorMode(qfw::TableView::EditRowsMixed);
	ui->tblRuns->setInlineEditStrategy(qfw::TableView::OnEditedValueCommit);
	m_runsTableItemDelegate = new RunsTableItemDelegate(ui->tblRuns);
	ui->tblRuns->setItemDelegate(m_runsTableItemDelegate);

	ui->tblRuns->setSelectionMode(QTableView::SingleSelection);
	ui->tblRuns->viewport()->setAcceptDrops(true);
	ui->tblRuns->setDropIndicatorShown(true);
	ui->tblRuns->setDragDropMode(QAbstractItemView::InternalMove);
	ui->tblRuns->setDragEnabled(false);

	auto m = new RunsTableModel(this);
	m->addColumn("id").setReadOnly(true);
	m->addColumn("classes.name", tr("Class"));
	m->addColumn("competitors.siId", tr("SI"));
	m->addColumn("competitorName", tr("Name"));
	m->addColumn("registration", tr("Reg"));
	m->addColumn("runs.siId", tr("SI"));
	m->addColumn("runs.startTimeMs", tr("Start")).setCastType(qMetaTypeId<quickevent::og::TimeMs>());
	m->addColumn("runs.timeMs", tr("Time")).setCastType(qMetaTypeId<quickevent::og::TimeMs>());
	qfm::SqlTableModel::ColumnDefinition::DbEnumCastProperties status_props;
	status_props.setGroupName("runs.status");
	m->addColumn("runs.status", tr("Status"))
			.setCastType(qMetaTypeId<qf::core::sql::DbEnum>())
			.setCastProperties(status_props);
	ui->tblRuns->setTableModel(m);
	m_runsModel = m;

	connect(m_runsModel, &RunsTableModel::startTimesSwitched, ui->tblRuns, &qf::qmlwidgets::TableView::reload);

	connect(ui->tblRuns->horizontalHeader(), &QHeaderView::sortIndicatorChanged, [this](int logical_index, Qt::SortOrder order)
	{
		auto cd = m_runsModel->columnDefinition(logical_index);
		bool is_sort_start_time_asc = (cd.matchesSqlId(QStringLiteral("runs.startTimeMs"))
									   && order == Qt::AscendingOrder
									   && ui->tblRuns->filterString().isEmpty());
		m_runsTableItemDelegate->setStartTimeHighlightVisible(is_sort_start_time_asc);
		ui->tblRuns->setDragEnabled(is_sort_start_time_asc);
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

void RunsWidget::reload()
{
	qfs::QueryBuilder qb;
	qb.select2("runs", "*")
			.select2("competitors", "registration, siId")
			.select2("classes", "name")
			.select("COALESCE(lastName, '') || ' ' || COALESCE(firstName, '') AS competitorName")
			.from("runs")
			.join("runs.competitorId", "competitors.id")
			.join("competitors.classId", "classes.id")
			.orderBy("runs.id");//.limit(10);
	int class_id = m_cbxClasses->currentData().toInt();
	if(class_id > 0) {
		qb.where("competitors.classId=" + QString::number(class_id));
	}
	m_runsTableItemDelegate->setHighlightedClassId(class_id);
	m_runsModel->setQueryBuilder(qb);
	m_runsModel->reload();
}

Event::EventPlugin *RunsWidget::eventPlugin()
{
	qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
	auto plugin = qobject_cast<Event::EventPlugin *>(fwk->plugin("Event"));
	QF_ASSERT(plugin != nullptr, "Bad plugin", return nullptr);
	return plugin;
}

void RunsWidget::settleDownInPartWidget(ThisPartWidget *part_widget)
{
	connect(part_widget, SIGNAL(resetPartRequest()), this, SLOT(reset()));
	connect(part_widget, SIGNAL(reloadPartRequest()), this, SLOT(reset()));
	/*
	qfw::Action *a = part_widget->menuBar()->actionForPath("station", true);
	a->setText("&Station");
	a->addActionInto(m_actCommOpen);
	*/
	qfw::ToolBar *main_tb = part_widget->toolBar("main", true);
	//main_tb->addAction(m_actCommOpen);
	{
		QLabel *lbl = new QLabel(tr("Class "));
		main_tb->addWidget(lbl);
	}
	{
		m_cbxClasses = new qfw::ForeignKeyComboBox();
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

void RunsWidget::reset()
{
	{
		m_cbxClasses->blockSignals(true);
		m_cbxClasses->loadItems(true);
		m_cbxClasses->insertItem(0, tr("--- all ---"), 0);
		connect(m_cbxClasses, SIGNAL(currentDataChanged(QVariant)), this, SLOT(reload()), Qt::UniqueConnection);
		m_cbxClasses->blockSignals(false);
	}
	reload();
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
			DrawMethod dm = DrawMethod(ui->cbxDrawMethod->currentData().toInt());
			if(dm == DrawMethod::RandomNumber) {
				qf::core::sql::QueryBuilder qb;
				qb.select2("classdefs", "startTimeMin, startIntervalMin, vacantsBefore, vacantEvery, vacantsAfter")
						.from("classdefs")
						.where("stageId=" QF_IARG(stage_id))
						.where("classId=" QF_IARG(class_id));
				qfs::Query q(transaction.connection());
				q.exec(qb.toString(), qf::core::Exception::Throw);
				if(q.next()) {
					int interval = q.value("startIntervalMin").toInt() * 60 * 1000;
					if(interval == 0) {
						if(!qf::qmlwidgets::dialogs::MessageBox::askYesNo(this, tr("Start interval is zero, proceed anyway?"), false))
							continue;
					}
					int start0 = q.value("startTimeMin").toInt() * 60 * 1000;
					int vacants_before = q.value("vacantsBefore").toInt();
					int vacant_every = q.value("vacantEvery").toInt();
					//int va = q.value("vacantsAfter").toInt();
					int start = start0 + vacants_before * interval;
					int n = 0;
					QMap<int, int> positions;
					qsrand(QTime::currentTime().msecsSinceStartOfDay());
					qb.clear();
					qb.select2("runs", "id")
							.from("competitors")
							.joinRestricted("competitors.id", "runs.competitorId", "runs.stageId=" QF_IARG(stage_id))
							.where("competitors.classId=" QF_IARG(class_id))
							.orderBy("runs.startTimeAlignment");
					q.exec(qb.toString(), qf::core::Exception::Throw);
					while(q.next()) {
						int rnd = qrand();
						positions[rnd] = q.value("runs.id").toInt();
					}
					q.prepare("UPDATE runs SET startTimeMs=:startTimeMs WHERE id=:id");
					QMapIterator<int, int> it(positions);
					while(it.hasNext()) {
						it.next();
						q.bindValue(QStringLiteral(":startTimeMs"), start);
						q.bindValue(QStringLiteral(":id"), it.value());
						q.exec(qf::core::Exception::Throw);
						start += interval;
						if(n > 0 && n % vacant_every == 0)
							start += interval;
					}
				}
			}
		}
		transaction.commit();
	}
	catch (const qf::core::Exception &e) {
		qf::qmlwidgets::dialogs::MessageBox::showException(this, e);
	}
	reload();
}


