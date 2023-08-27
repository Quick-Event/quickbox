#include "competitorwidget.h"
#include "ui_competitorwidget.h"

#include "competitordocument.h"

#include <plugins/Event/src/eventplugin.h>
#include <plugins/Runs/src/cardflagsdialog.h>
#include <plugins/Runs/src/runflagsdialog.h>
#include <plugins/Runs/src/runsplugin.h>

#include <quickevent/gui/og/itemdelegate.h>
#include <quickevent/core/og/sqltablemodel.h>
#include <quickevent/core/og/timems.h>
#include <quickevent/core/si/siid.h>

#include <qf/qmlwidgets/dialogs/dialog.h>
#include <qf/qmlwidgets/dialogs/messagebox.h>
#include <qf/qmlwidgets/framework/mainwindow.h>

#include <qf/core/sql/dbenum.h>
#include <qf/core/sql/transaction.h>
#include <qf/core/sql/query.h>
#include <qf/core/assert.h>

#include <QMenu>
#include <QAction>
#include <QCompleter>
#include <QDate>
#include <QPushButton>

namespace qfd = qf::qmlwidgets::dialogs;
namespace qfw = qf::qmlwidgets;
namespace qfc = qf::core;
namespace qfs = qf::core::sql;
using qf::qmlwidgets::framework::getPlugin;
using Event::EventPlugin;
using Runs::RunsPlugin;

namespace {

class CompetitorRunsModel : public quickevent::core::og::SqlTableModel
{
	Q_DECLARE_TR_FUNCTIONS(RunsModel)
private:
	using Super = quickevent::core::og::SqlTableModel;
public:
	CompetitorRunsModel(QObject *parent = nullptr);

	enum Columns {
		col_runs_isRunning = 0,
		col_runs_stageId,
		col_classes_name,
		col_relays_name,
		col_runs_leg,
		col_runs_siId,
		col_runs_startTimeMs,
		col_runs_timeMs,
		col_runs_runFlags,
		col_runs_cardFlags,
		col_COUNT
	};
private:
	QVariant value(int row_ix, int column_ix) const override;
};

CompetitorRunsModel::CompetitorRunsModel(QObject *parent)
	: Super(parent)
{
	clearColumns(col_COUNT);
	setColumn(col_runs_isRunning, ColumnDefinition("runs.isRunning", tr("Running", "runs.isRunning")).setToolTip(tr("Is running")));
	setColumn(col_runs_stageId, ColumnDefinition("runs.stageId", tr("Stage")).setReadOnly(true));
	setColumn(col_relays_name, ColumnDefinition("relayName", tr("Relay")).setReadOnly(true));
	setColumn(col_classes_name, ColumnDefinition("classes.name", tr("Class")).setReadOnly(true));
	setColumn(col_runs_leg, ColumnDefinition("runs.leg", tr("Leg")));
	setColumn(col_runs_siId, ColumnDefinition("runs.siid", tr("SI")).setReadOnly(false).setCastType(qMetaTypeId<quickevent::core::si::SiId>()));
	setColumn(col_runs_startTimeMs, ColumnDefinition("runs.startTimeMs", tr("Start")).setCastType(qMetaTypeId<quickevent::core::og::TimeMs>()).setReadOnly(true));
	setColumn(col_runs_timeMs, ColumnDefinition("runs.timeMs", tr("Time")).setCastType(qMetaTypeId<quickevent::core::og::TimeMs>()).setReadOnly(true));
	setColumn(col_runs_runFlags, ColumnDefinition("runFlags", tr("Run flags")).setReadOnly(true));
	setColumn(col_runs_cardFlags, ColumnDefinition("cardFlags", tr("Card flags")).setReadOnly(true));
}

}

CompetitorWidget::CompetitorWidget(QWidget *parent) :
	Super(parent),
	ui(new Ui::CompetitorWidget)
{
	qfLogFuncFrame();
	bool is_relays = getPlugin<EventPlugin>()->eventConfig()->isRelays();

	setPersistentSettingsId("CompetitorWidget");
	ui->setupUi(this);

	ui->chkFind->setChecked(true);

	setTitle(tr("Competitor"));

	{
		qf::qmlwidgets::ForeignKeyComboBox *cbx = ui->cbxClass;
		if(is_relays) {
			cbx->setEnabled(false);
		}
		else {
			cbx->setReferencedTable("classes");
			cbx->setReferencedField("id");
			cbx->setReferencedCaptionField("name");
		}
	}

	connect(ui->edFind, &FindRegistrationEdit::registrationSelected, this, &CompetitorWidget::onRegistrationSelected);

	dataController()->setDocument(new Competitors::CompetitorDocument(this));
	m_runsModel = (RunsTableModel*) new CompetitorRunsModel(this);
	ui->tblRuns->setTableModel(m_runsModel);
	ui->tblRuns->setPersistentSettingsId(ui->tblRuns->objectName());
	ui->tblRuns->setInlineEditSaveStrategy(qf::qmlwidgets::TableView::OnManualSubmit);
	ui->tblRuns->setItemDelegate(new quickevent::gui::og::ItemDelegate(ui->tblRuns));

	ui->tblRuns->horizontalHeader()->setSectionHidden(CompetitorRunsModel::col_relays_name, !is_relays);
	ui->tblRuns->horizontalHeader()->setSectionHidden(CompetitorRunsModel::col_runs_leg, !is_relays);
	ui->tblRuns->horizontalHeader()->setSectionHidden(CompetitorRunsModel::col_classes_name, !is_relays);
	//ui->tblRuns->setContextMenuPolicy(Qt::CustomContextMenu);
	//connect(ui->tblRuns, &qfw::TableView::customContextMenuRequested, this, &CompetitorWidget::onRunsTableCustomContextMenuRequest);

	{
		int stage_cnt = getPlugin<EventPlugin>()->stageCount();
		auto *ly = new QHBoxLayout(ui->grpStartTimes);
		for (int i = 1; i <= stage_cnt; ++i) {
			QPushButton *bt = new QPushButton(tr("E&%1").arg(i));
			ly->addWidget(bt);
			connect(bt, &QPushButton::clicked, this, [this, i]() {
				this->showRunsTable(i);
			});
		}
	}

	// if there is only one run propagate widget SI card change from competitors to runs
	connect(ui->edSiId, qOverload<int>(&QSpinBox::valueChanged), this, [this](int new_si_number) // widget SIcard edit box
	{
		if(getPlugin<EventPlugin>()->stageCount() == 1 && m_runsModel->rowCount() == 1 ) {
			m_runsModel->setValue(0, CompetitorRunsModel::col_runs_siId, new_si_number); // update SI in runs model
			ui->tblRuns->reset(); // reload ui to see the change
		}
	});

	connect(ui->tblRuns, &qfw::TableView::editCellRequest, this, [this](QModelIndex index) {
		auto col = index.column();
		if(col == CompetitorRunsModel::col_runs_runFlags) {
			Runs::RunFlagsDialog dlg(this);
			dlg.load(m_runsModel, ui->tblRuns->toTableModelRowNo(ui->tblRuns->currentIndex().row()));
			if(dlg.exec()) {
				dlg.save();
			}
		} else if(col == CompetitorRunsModel::col_runs_cardFlags) {
			Runs::CardFlagsDialog dlg(this);
			dlg.load(m_runsModel, ui->tblRuns->toTableModelRowNo(ui->tblRuns->currentIndex().row()));
			if(dlg.exec()) {
				dlg.save();
			}
		}
	}, Qt::QueuedConnection);
}

CompetitorWidget::~CompetitorWidget()
{
	delete ui;
}

bool CompetitorWidget::loadRunsTable()
{
	//bool is_relays = getPlugin<EventPlugin>()->eventConfig()->isRelays();
	qf::core::model::DataDocument *doc = dataController()->document();
	qf::core::sql::QueryBuilder qb;
	qb.select2("runs", "*")
			.select2("classes", "name")
			.select("lentcards.siid IS NOT NULL AS cardInLentTable")
			.select("COALESCE(relays.club, '') || ' ' || COALESCE(relays.name, '') AS relayName")
			.select("'' AS runFlags")
			.select("'' AS cardFlags")
			.from("runs")
			.join("runs.competitorId", "competitors.id")
			.join("runs.relayId", "relays.id")
			.join("relays.classId", "classes.id")
			.joinRestricted("runs.siid", "lentcards.siid", "NOT lentcards.ignored")
			.where("runs.competitorId=" QF_IARG(doc->value("competitors.id").toInt()))
			.orderBy("runs.stageId");
	m_runsModel->setQueryBuilder(qb, false);
	return m_runsModel->reload();
}

bool CompetitorWidget::saveRunsTable()
{
	qfLogFuncFrame();
	/*
	bool is_running_set = false;
	for (int i = 0; i < m_runsModel->rowCount(); ++i) {
		bool is_running = m_runsModel->value(i, RunsModel::col_runs_isRunning).toBool();
		int time_ms = m_runsModel->value(i, RunsModel::col_runs_timeMs).toInt();
		if(!is_running && time_ms > 0) {
			m_runsModel->setData(m_runsModel->index(i, RunsModel::col_runs_isRunning), true);
			is_running_set = true;
		}
	}
	if(is_running_set)
		throw BadDataInputException(tr("Canont set not running flag for competitor with valid finish time."));
	*/
	bool ret = m_runsModel->postAll(true);
	if(ret)
		getPlugin<EventPlugin>()->emitDbEvent(Event::EventPlugin::DBEVENT_COMPETITOR_COUNTS_CHANGED);
	return ret;
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
	else if(mode == qf::core::model::DataDocument::ModeView || mode == qf::core::model::DataDocument::ModeDelete) {
		ui->frmFind->hide();
	}
	if(Super::load(id, mode))
		return loadRunsTable();
	return false;
}

QString CompetitorWidget::guessClassFromRegistration(const QString &registration)
{
	// get list of classes without preceding letter - eg. 12,14,18,21,35,..
	QVector<int> classes;
	for (int i = 0; i < ui->cbxClass->count(); ++i)
	{
		QString class_name = ui->cbxClass->itemText(i);
		int age = class_name.mid(1, 2).toInt();
		classes << age;
	}
	std::sort(classes.begin(), classes.end());

	// get runner age
	qfLogFuncFrame() << registration;
	int curr_year = QDate::currentDate().year();
	int runner_age = curr_year - 1900 - registration.mid(3, 2).toInt();
	if(runner_age >= 100)
		runner_age -= 100;
	qfDebug() << "\t age:" << runner_age;

	// try to guess gender prefix - D or H
	char gender = (registration.mid(5, 1).toInt() >= 5)? 'D': 'H';

	// go trough classes, if runner age >= class then asign
	// reverse array order and comparison for juniors
	// eg. classes 35, 40, 45, age 38 -> 35
	// eg. classes 12, 14, 16, age 15 -> 16
	int candidate = 0;
	if (runner_age > 21)
	{
		for(int cls : classes) {
			if(runner_age >= cls)
				candidate = cls;
		}
	}
	else
	{
		std::reverse(classes.begin(), classes.end());
		for(int cls : classes) {
			if(runner_age <= cls)
				candidate = cls;
		}
	}
	return candidate ? gender + QString::number(candidate) : QString();
}

void CompetitorWidget::showRunsTable(int stage_id)
{
	if(!saveData())
		return;

	qf::core::model::DataDocument*doc = dataController()->document();
	int competitor_id = doc->value("competitors.id").toInt();
	int class_id = ui->cbxClass->currentData().toInt();
	QString sort_col = QStringLiteral("runs.startTimeMs");
	getPlugin<RunsPlugin>()->showRunsTable(stage_id, class_id, false, sort_col, competitor_id);
	loadRunsTable();
}

void CompetitorWidget::onRegistrationSelected(const QVariantMap &values)
{
	qfLogFuncFrame();
	auto *doc = dataController()->document();
	for(auto s : {"firstname", "lastname", "registration", "licence", "siid"}) {
		qfDebug() << "\t" << s << "->" << values.value(s);
		doc->setValue(s, values.value(s));
	}
	bool is_relays = getPlugin<EventPlugin>()->eventConfig()->isRelays();
	if(!is_relays) {
		// if no class is set, guess class from registration
		if(ui->cbxClass->currentText().isEmpty()) {
			QString class_name_prefix = guessClassFromRegistration(values.value("registration").toString());
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
	}
}

void CompetitorWidget::loadFromRegistrations(int siid)
{
	qfs::Query q;
	q.exec("SELECT * FROM registrations WHERE siId=" + QString::number(siid), qfc::Exception::Throw);
	if(q.next()) {
		QVariantMap vals = q.values();
		onRegistrationSelected(vals);
	}
	else {
		dataController()->document()->setValue(QStringLiteral("competitors.siid"), siid);
	}
}

bool CompetitorWidget::saveData()
{
	try {
		bool is_relays = getPlugin<EventPlugin>()->eventConfig()->isRelays();
		Competitors::CompetitorDocument*doc = qobject_cast<Competitors::CompetitorDocument*>(dataController()->document());
		if(!is_relays && doc->value(QStringLiteral("classId")).toInt() == 0) {
			qf::qmlwidgets::dialogs::MessageBox::showWarning(this, tr("Class should be entered."));
			return false;
		}
		if(Super::saveData())
			return saveRunsTable();
	}
	catch (const qf::core::Exception &e) {
		QMessageBox::warning(this, tr("SQL error"), e.message());
	}
	return false;
}

QVariant CompetitorRunsModel::value(int row_ix, int column_ix) const
{
	if(column_ix == col_runs_runFlags) {
		qf::core::utils::TableRow row = tableRow(row_ix);
		bool is_disqualified = row.value(QStringLiteral("runs.disqualified")).toBool();
		bool is_disqualified_by_organizer = row.value(QStringLiteral("runs.disqualifiedByOrganizer")).toBool();
		bool mis_punch = row.value(QStringLiteral("runs.misPunch")).toBool();
		bool bad_check = row.value(QStringLiteral("runs.badCheck")).toBool();
		bool not_start = row.value(QStringLiteral("runs.notStart")).toBool();
		bool not_finish = row.value(QStringLiteral("runs.notFinish")).toBool();
		bool not_competing = row.value(QStringLiteral("runs.notCompeting")).toBool();
		QStringList sl;
		if(is_disqualified)
			sl << tr("DISQ", "Disqualified");
		if(is_disqualified_by_organizer)
			sl << tr("DO", "disqualifiedByOrganizer");
		if(mis_punch)
			sl << tr("MP", "MisPunch");
		if(bad_check)
			sl << tr("BC", "BadCheck");
		if(not_competing)
			sl << tr("NC", "NotCompeting");
		if(not_start)
			sl << tr("DNS", "DidNotStart");
		if(not_finish)
			sl << tr("DNF", "DidNotFinish");
		if(sl.isEmpty())
			return QStringLiteral("");
		else
			return sl.join(',');
	}
	else if(column_ix == col_runs_cardFlags) {
		qf::core::utils::TableRow row = tableRow(row_ix);
		bool card_rent_requested = row.value(QStringLiteral("runs.cardLent")).toBool();
		bool card_returned = row.value(QStringLiteral("runs.cardReturned")).toBool();
		bool card_in_lent_table = row.value(QStringLiteral("cardInLentTable")).toBool();
		QStringList sl;
		if(card_rent_requested)
			sl << tr("CR", "Card rent requested");
		if(card_in_lent_table)
			sl << tr("CT", "Card in lent cards table");
		if(card_returned)
			sl << tr("RET", "Card returned");
		if(sl.isEmpty())
			return QStringLiteral("");
		else
			return sl.join(',');
	}
	return Super::value(row_ix, column_ix);
}

