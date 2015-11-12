#include "competitorwidget.h"
#include "ui_competitorwidget.h"

#include "Competitors/competitordocument.h"
#include "Competitors/competitorsplugin.h"
#include "registrationswidget.h"

#include "Event/eventplugin.h"

#include <quickevent/og/itemdelegate.h>
#include <quickevent/og/sqltablemodel.h>
#include <quickevent/og/timems.h>

#include <qf/qmlwidgets/dialogs/dialog.h>
#include <qf/qmlwidgets/dialogs/messagebox.h>
#include <qf/qmlwidgets/framework/mainwindow.h>

#include <qf/core/sql/dbenum.h>

#include <QMenu>
#include <QAction>
#include <QCompleter>
#include <QDate>
#include <QPushButton>

namespace qfd = qf::qmlwidgets::dialogs;
namespace qfw = qf::qmlwidgets;
/*
static Competitors::CompetitorsPlugin* competitorsPlugin()
{
	qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
	qf::qmlwidgets::framework::Plugin *plugin = fwk->plugin("Competitors");
	return qobject_cast<Competitors::CompetitorsPlugin *>(plugin);
}

*/

static Event::EventPlugin* eventPlugin()
{
	qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
	auto *plugin = qobject_cast<Event::EventPlugin*>(fwk->plugin("Event"));
	QF_ASSERT_EX(plugin != nullptr, "Bad event plugin!");
	return plugin;
}

CompetitorWidget::CompetitorWidget(QWidget *parent) :
	Super(parent),
	ui(new Ui::CompetitorWidget)
{
	qfLogFuncFrame();
	setPersistentSettingsId("CompetitorWidget");
	ui->setupUi(this);

	ui->chkFind->setChecked(true);

	setTitle(tr("competitor"));

	{
		qf::qmlwidgets::ForeignKeyComboBox *cbx = ui->cbxClass;
		cbx->setReferencedTable("classes");
		cbx->setReferencedField("id");
		cbx->setReferencedCaptionField("name");
	}

	connect(ui->edFind, &FindRegistrationEdit::registrationSelected, this, &CompetitorWidget::onRegistrationSelected);

	dataController()->setDocument(new Competitors::CompetitorDocument(this));
	m_runsModel = new quickevent::og::SqlTableModel(this);
	m_runsModel->addColumn("runs.offRace", tr("Off", "runs.offRace")).setToolTip(tr("Off race in this stage"));
	m_runsModel->addColumn("runs.stageId", tr("Stage")).setReadOnly(true);
	m_runsModel->addColumn("runs.siid", tr("SI")).setReadOnly(false).setCastType(qMetaTypeId<quickevent::og::SiId>());
	m_runsModel->addColumn("runs.startTimeMs", tr("Start")).setCastType(qMetaTypeId<quickevent::og::TimeMs>());
	m_runsModel->addColumn("runs.timeMs", tr("Time"))
			.setCastType(qMetaTypeId<quickevent::og::TimeMs>());
	m_runsModel->addColumn("runs.notCompeting", tr("NC", "runs.notCompeting")).setToolTip(tr("Not competing"));
	m_runsModel->addColumn("runs.disqualified", tr("D", "runs.disqualified")).setToolTip(tr("Disqualified in this stage"));
	m_runsModel->addColumn("runs.misPunch", tr("E", "runs.misPunch")).setToolTip(tr("Card mispunch in this stage"));
	m_runsModel->addColumn("runs.cardLent", tr("L", "runs.cardLent")).setToolTip(tr("Card lent in this stage"));
	/*
	quickevent::og::SqlTableModel::ColumnDefinition::DbEnumCastProperties runs_status_db_enum_cast_props;
	runs_status_db_enum_cast_props.setGroupName("runs.status");
	m_runsModel->addColumn("runs.status", tr("Status"))
			.setToolTip(tr("Run status in this stage"))
			.setCastType(qMetaTypeId<qf::core::sql::DbEnum>(), runs_status_db_enum_cast_props);
	*/
	ui->tblRuns->setTableModel(m_runsModel);
	ui->tblRuns->setPersistentSettingsId(ui->tblRuns->objectName());
	ui->tblRuns->setInlineEditSaveStrategy(qf::qmlwidgets::TableView::OnManualSubmit);
	ui->tblRuns->setItemDelegate(new quickevent::og::ItemDelegate(ui->tblRuns));
	//ui->tblRuns->setContextMenuPolicy(Qt::CustomContextMenu);
	//connect(ui->tblRuns, &qfw::TableView::customContextMenuRequested, this, &CompetitorWidget::onRunsTableCustomContextMenuRequest);

	{
		int stage_cnt = eventPlugin()->stageCount();
		auto *ly = new QHBoxLayout(ui->grpStartTimes);
		for (int i = 1; i <= stage_cnt; ++i) {
			QPushButton *bt = new QPushButton(tr("E&%1").arg(i));
			ly->addWidget(bt);
			connect(bt, &QPushButton::clicked, [this, i]() {
				this->showRunsTable(i);
			});
		}
	}
}

CompetitorWidget::~CompetitorWidget()
{
	delete ui;
}

bool CompetitorWidget::loadRunsTable()
{
	qf::core::model::DataDocument *doc = dataController()->document();
	qf::core::sql::QueryBuilder qb;
	qb.select2("runs", "*")
			.select2("competitors", "classId")
			.from("runs")
			.join("runs.competitorId", "competitors.id")
			.where("runs.competitorId=" QF_IARG(doc->value("competitors.id").toInt()))
			.orderBy("runs.stageId");
	m_runsModel->setQueryBuilder(qb);
	return m_runsModel->reload();
}

bool CompetitorWidget::saveRunsTable()
{
	qfLogFuncFrame();
	return m_runsModel->postAll(true);
}
/*
void CompetitorWidget::onRunsTableCustomContextMenuRequest(const QPoint &pos)
{
	qfLogFuncFrame();
	QAction a_show_in_runs(tr("Show in runs table"), nullptr);
	QList<QAction*> lst;
	lst << &a_show_in_runs;
	QAction *a = QMenu::exec(lst, ui->tblRuns->viewport()->mapToGlobal(pos));
	if(a == &a_show_in_runs) {
		auto row = ui->tblRuns->tableRow();
		int stage_no = row.value("stageId").toInt();
		int class_id = row.value("classId").toInt();
		int competitor_id = row.value("competitorId").toInt();
		//QMetaObject::invokeMethod(this, "accept", Qt::QueuedConnection);
		emit editStartListRequest(stage_no, class_id, competitor_id);
	}
}
*/
bool CompetitorWidget::load(const QVariant &id, int mode)
{
	ui->chkFind->setChecked(mode == qf::core::model::DataDocument::ModeInsert);
	if(mode == qf::core::model::DataDocument::ModeInsert) {
		ui->edFind->setFocus();
	}
	if(Super::load(id, mode))
		return loadRunsTable();
	return false;
}

QString CompetitorWidget::classNameFromRegistration(const QString &registration)
{
	qfLogFuncFrame() << registration;
	QString reg = registration.mid(3);
	int year = registration.mid(3, 2).toInt() + 1900;
	int curr_year = QDate::currentDate().year();
	int age = curr_year - year;
	if(age >= 100)
		age -= 100;
	qfDebug() << "\t age:" << age;
	QChar c = (registration.mid(5, 1).toInt() == 5)? 'D': 'H';
	for(int y : juniorAges()) {
		if(y >= age)
			return c + QString::number(y);
	}
	for(int y : veteranAges()) {
		if(age >= y)
			return c + QString::number(y);
	}
	return QString();
}

void CompetitorWidget::showRunsTable(int stage_id)
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
								  , Q_ARG(QString, sort_col)
								  , Q_ARG(int, competitor_id));
		loadRunsTable();
	}
}

void CompetitorWidget::onRegistrationSelected(const QVariantMap &values)
{
	qfLogFuncFrame();
	qf::core::model::DataDocument*doc = dataController()->document();
	for(auto s : {"firstname", "lastname", "registration", "licence", "siid"}) {
		qfDebug() << "\t" << s << "->" << values.value(s);
		doc->setValue(s, values.value(s));
	}
	QString class_name_prefix = classNameFromRegistration(values.value("registration").toString());
	if(!class_name_prefix.isEmpty()) {
		for (int i = 0; i < ui->cbxClass->count(); ++i) {
			QString class_name = ui->cbxClass->itemText(i);
			if(class_name.startsWith(class_name_prefix)) {
				ui->cbxClass->setCurrentText(class_name);
				break;
			}
		}
	}
}

bool CompetitorWidget::saveData()
{
	try {
		if(Super::saveData())
			return saveRunsTable();
	}
	catch (qf::core::Exception &e) {
		qf::qmlwidgets::dialogs::MessageBox::showException(this, e);
	}
	return false;
}

QVector<int> CompetitorWidget::juniorAges()
{
	QVector<int> ret;
	for (int i = 0; i < ui->cbxClass->count(); ++i) {
		QString class_name = ui->cbxClass->itemText(i);
		int age = class_name.mid(1, 2).toInt();
		if(age > 0 && age < 21)
			ret << age;
	}
	std::sort(ret.begin(), ret.end());
	return ret;
}

QVector<int> CompetitorWidget::veteranAges()
{
	QVector<int> ret;
	for (int i = 0; i < ui->cbxClass->count(); ++i) {
		QString class_name = ui->cbxClass->itemText(i);
		int age = class_name.mid(1, 2).toInt();
		if(age > 0 && age >= 21)
			ret << age;
	}
	std::sort(ret.begin(), ret.end(), std::greater<int>());
	return ret;
}

