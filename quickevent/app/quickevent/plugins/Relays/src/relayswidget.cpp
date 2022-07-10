#include "relayswidget.h"
#include "ui_relayswidget.h"
#include "relaywidget.h"

#include "relaydocument.h"
#include "relaysplugin.h"

#include <plugins/Event/src/eventplugin.h>
#include <plugins/Runs/src/runsplugin.h>

#include <quickevent/gui/partwidget.h>
#include <quickevent/gui/reportoptionsdialog.h>

#include <quickevent/core/si/siid.h>
#include <quickevent/core/utils.h>
#include <quickevent/core/si/punchrecord.h>

#include <qf/qmlwidgets/dialogs/dialog.h>
#include <qf/qmlwidgets/dialogs/getiteminputdialog.h>
#include <qf/qmlwidgets/dialogs/filedialog.h>
#include <qf/qmlwidgets/dialogs/messagebox.h>
#include <qf/qmlwidgets/framework/mainwindow.h>
#include <qf/qmlwidgets/framework/plugin.h>
#include <qf/qmlwidgets/toolbar.h>
#include <qf/qmlwidgets/combobox.h>
#include <qf/qmlwidgets/action.h>
#include <qf/qmlwidgets/menubar.h>
#include <qf/qmlwidgets/dialogbuttonbox.h>
#include <qf/qmlwidgets/reports/widgets/reportviewwidget.h>

#include <qf/core/model/sqltablemodel.h>
#include <qf/core/sql/querybuilder.h>
#include <qf/core/sql/transaction.h>
#include <qf/core/assert.h>
#include <qf/core/utils/htmlutils.h>
#include <qf/core/utils/treetable.h>
#include <QCheckBox>
#include <QInputDialog>
#include <QLabel>
#include <QProgressDialog>
#include <QPushButton>
#include <QTimer>

//#define QF_TIMESCOPE_ENABLED
#include <qf/core/utils/timescope.h>

namespace qfs = qf::core::sql;
namespace qfw = qf::qmlwidgets;
namespace qff = qf::qmlwidgets::framework;
namespace qfd = qf::qmlwidgets::dialogs;
namespace qfc = qf::core;
namespace qfm = qf::core::model;
using qf::qmlwidgets::framework::getPlugin;
using Event::EventPlugin;
using Runs::RunsPlugin;
using Relays::RelaysPlugin;

namespace {

static QString datetime_to_string(const QDateTime &dt)
{
	return quickevent::core::Utils::dateTimeToIsoStringWithUtcOffset(dt);
}

enum Columns {
	col_relays_id = 0,
	col_classes_name,
	col_relays_club,
	col_relays_name,
	col_relays_number,
	col_relays_note,
	col_COUNT
};
}// namespace

RelaysWidget::RelaysWidget(QWidget *parent) :
	Super(parent),
	ui(new Ui::RelaysWidget)
{
	ui->setupUi(this);

	ui->tblRelaysToolBar->setTableView(ui->tblRelays);

	ui->tblRelays->setCloneRowEnabled(false);
	ui->tblRelays->setDirtyRowsMenuSectionEnabled(false);
	ui->tblRelays->setPersistentSettingsId("tblRelays");
	ui->tblRelays->setRowEditorMode(qfw::TableView::EditRowsMixed);
	ui->tblRelays->setInlineEditSaveStrategy(qfw::TableView::OnEditedValueCommit);
	qfm::SqlTableModel *m = new qfm::SqlTableModel(this);
	using CD = qfm::TableModel::ColumnDefinition;
	m->clearColumns(col_COUNT);
	m->setColumn(col_relays_id, CD("id").setReadOnly(true));
	m->setColumn(col_classes_name, CD("classes.name", tr("Class")).setReadOnly(true));
	m->setColumn(col_relays_club, CD("club", tr("Club")));
	m->setColumn(col_relays_name, CD("name", tr("Name")));
	m->setColumn(col_relays_number, CD("number", tr("Number")));
	m->setColumn(col_relays_note, CD("note", tr("Note")));
	ui->tblRelays->setTableModel(m);
	m_tblModel = m;

	//connect(ui->tblRelays, SIGNAL(editRowInExternalEditor(QVariant,int)), this, SLOT(edit Relay(QVariant,int)), Qt::QueuedConnection);
	connect(ui->tblRelays, &qfw::TableView::editRowInExternalEditor, this, &RelaysWidget::editRelay, Qt::QueuedConnection);
	connect(ui->tblRelays, &qfw::TableView::editSelectedRowsInExternalEditor, this, &RelaysWidget::editRelays, Qt::QueuedConnection);

	//connect(getPlugin<EventPlugin>(), &Relays::RelaysPlugin::dbEventNotify, this, &RelaysWidget::onDbEventNotify);

	QMetaObject::invokeMethod(this, "lazyInit", Qt::QueuedConnection);
}

RelaysWidget::~RelaysWidget()
{
	delete ui;
}

void RelaysWidget::settleDownInPartWidget(quickevent::gui::PartWidget *part_widget)
{
	connect(part_widget, SIGNAL(resetPartRequest()), this, SLOT(reset()));
	connect(part_widget, SIGNAL(reloadPartRequest()), this, SLOT(reload()));
	qfw::ToolBar *main_tb = part_widget->toolBar("main", true);
	{
		QLabel *lbl;
		{
			lbl = new QLabel(tr("&Class "));
			main_tb->addWidget(lbl);
		}
		{
			m_cbxClasses = new qfw::ForeignKeyComboBox();
#if QT_VERSION >= QT_VERSION_CHECK(5, 11, 0)
			m_cbxClasses->setMinimumWidth(fontMetrics().horizontalAdvance('X') * 10);
#else
			m_cbxClasses->setMinimumWidth(fontMetrics().width('X') * 10);
#endif
			m_cbxClasses->setMaxVisibleItems(100);
			m_cbxClasses->setReferencedTable("classes");
			m_cbxClasses->setReferencedField("id");
			m_cbxClasses->setReferencedCaptionField("name");
			main_tb->addWidget(m_cbxClasses);
		}
		lbl->setBuddy(m_cbxClasses);
	}
	/*
	main_tb->addSeparator();
	{
		m_cbxEditRelayOnPunch = new QCheckBox(tr("Edit on punch"));
		m_cbxEditRelayOnPunch->setToolTip(tr("Edit or insert competitor on card insert into station."));
		main_tb->addWidget(m_cbxEditRelayOnPunch);
	}
	*/
	qfw::Action *a_relays = part_widget->menuBar()->actionForPath("relay");
	a_relays->setText(tr("&Relays"));
	{
		qfw::Action *a = new qfw::Action("assignNumbers", tr("&Assign numbers"));
		a_relays->addActionInto(a);
		connect(a, &qfw::Action::triggered, this, &RelaysWidget::relays_assignNumbers);
	}

	qfw::Action *a_print = part_widget->menuBar()->actionForPath("print");
	a_print->setText(tr("&Print"));

	qfw::Action *a_print_start_list = a_print->addMenuInto("startList", tr("&Start list"));
	qfw::Action *a_print_start_list_classes = new qfw::Action("classes", tr("&Classes"));
	a_print_start_list->addActionInto(a_print_start_list_classes);
	connect(a_print_start_list_classes, &qfw::Action::triggered, this, &RelaysWidget::print_start_list_classes);
	qfw::Action *a_print_start_list_clubs = new qfw::Action("clubs", tr("C&lubs"));
	a_print_start_list->addActionInto(a_print_start_list_clubs);
	connect(a_print_start_list_clubs, &qfw::Action::triggered, this, &RelaysWidget::print_start_list_clubs);

	qfw::Action *a_print_results = a_print->addMenuInto("results", tr("&Results"));
	{
		qfw::Action *a = new qfw::Action("nlegs", tr("&After n legs"));
		a_print_results->addActionInto(a);
		connect(a, &qfw::Action::triggered, this, &RelaysWidget::print_results_nlegs);
	}
	{
		qfw::Action *a = new qfw::Action("nlegs", tr("&Overall"));
		a_print_results->addActionInto(a);
		connect(a, &qfw::Action::triggered, this, &RelaysWidget::print_results_overal);
	}
	{
		qfw::Action *a = new qfw::Action("nlegs", tr("Overall condensed"));
		a_print_results->addActionInto(a);
		connect(a, &qfw::Action::triggered, this, &RelaysWidget::print_results_overal_condensed);
	}

	qfw::Action *a_export = part_widget->menuBar()->actionForPath("export");
	a_export->setText(tr("E&xport"));
	qfw::Action *a_export_results = a_export->addMenuInto("export", tr("&Results"));
	{
		qfw::Action *a = new qfw::Action("exportIofXml3", tr("IOF-XML 3.0"));
		a_export_results->addActionInto(a);
		connect(a, &qfw::Action::triggered, this, &RelaysWidget::export_results_iofxml3);
	}
}

void RelaysWidget::lazyInit()
{
}

void RelaysWidget::reset()
{
	if(!getPlugin<EventPlugin>()->isEventOpen()) {
		m_tblModel->clearRows();
		return;
	}
	{
		m_cbxClasses->blockSignals(true);
		m_cbxClasses->loadItems(true);
		m_cbxClasses->insertItem(0, tr("--- all ---"), 0);
		m_cbxClasses->setCurrentIndex(0);
		connect(m_cbxClasses, SIGNAL(currentDataChanged(QVariant)), this, SLOT(reload()), Qt::UniqueConnection);
		m_cbxClasses->blockSignals(false);
	}
	reload();
}

void RelaysWidget::reload()
{
	qfs::QueryBuilder qb;
	qb.select2("relays", "*")
			.select2("classes", "name")
			.from("relays")
			.join("relays.classId", "classes.id")
			.orderBy("classes.name, relays.name");//.limit(10);
	int class_id = m_cbxClasses->currentData().toInt();
	if(class_id > 0) {
		qb.where("relays.classId=" + QString::number(class_id));
	}
	m_tblModel->setQueryBuilder(qb, false);
	m_tblModel->reload();
}

void RelaysWidget::editRelay(const QVariant &id, int mode)
{
	qfLogFuncFrame() << "id:" << id << "mode:" << mode;
	//qf::core::sql::Transaction transaction;
	auto *w = new  RelayWidget();
	w->setWindowTitle(tr("Edit Relay"));
	qfd::Dialog dlg(QDialogButtonBox::Save | QDialogButtonBox::Cancel, this);
	dlg.setDefaultButton(QDialogButtonBox::Save);
	QPushButton *bt_save_and_next = dlg.buttonBox()->addButton(tr("Save and &next"), QDialogButtonBox::AcceptRole);
	bool save_and_next = false;
	connect(dlg.buttonBox(), &qf::qmlwidgets::DialogButtonBox::clicked, [&save_and_next, bt_save_and_next](QAbstractButton *button) {
		save_and_next = (button == bt_save_and_next);
	});
	dlg.setCentralWidget(w);
	w->load(id, mode);
	auto *doc = qobject_cast<Relays:: RelayDocument*>(w->dataController()->document());
	QF_ASSERT(doc != nullptr, "Document is null!", return);
	if(mode == qfm::DataDocument::ModeInsert) {
		int class_id = m_cbxClasses->currentData().toInt();
		doc->setValue("relays.classId", class_id);
	}
	connect(doc, &Relays:: RelayDocument::saved, ui->tblRelays, &qf::qmlwidgets::TableView::rowExternallySaved, Qt::QueuedConnection);
	bool ok = dlg.exec();
	//if(ok)
	//	transaction.commit();
	//else
	//	transaction.rollback();
	if(ok && save_and_next) {
		QTimer::singleShot(0, [this]() {
			this->editRelay(QVariant(), qf::core::model::DataDocument::ModeInsert);
		});
	}
}

void RelaysWidget::editRelays(int mode)
{
	if(mode == qfm::DataDocument::ModeDelete) {
		QList<int> sel_rows = ui->tblRelays->selectedRowsIndexes();
		if(sel_rows.count() <= 1)
			return;
		if(qfd::MessageBox::askYesNo(this, tr("Really delete all the selected relays? This action cannot be reverted."), false)) {
			qfs::Transaction transaction;
			int n = 0;
			for(int ix : sel_rows) {
				int id = ui->tblRelays->tableRow(ix).value(ui->tblRelays->idColumnName()).toInt();
				if(id > 0) {
					Relays:: RelayDocument doc;
					doc.load(id, qfm::DataDocument::ModeDelete);
					doc.drop();
					n++;
				}
			}
			if(n > 0) {
				if(qfd::MessageBox::askYesNo(this, tr("Confirm deletion of %1 relays.").arg(n), false)) {
					transaction.commit();
					ui->tblRelays->reload();
				}
				else {
					transaction.rollback();
				}
			}
		}
	}
}
/*
void RelaysWidget::onDbEventNotify(const QString &domain, int connection_id, const QVariant &data)
{
	Q_UNUSED(connection_id)
	qfLogFuncFrame() << "domain:" << domain << "payload:" << data;
}
*/
void RelaysWidget::relays_assignNumbers()
{
	int method = qfd::GetItemInputDialog::getItem(this, tr("Dialog"), tr("Assign relay numbers method")
									 , QStringList{tr("Random number"), tr("In alphabetical order")}
									 , -1);
	if(method < 0)
		return;
	qf::core::sql::Transaction transaction;
	int class_id = m_cbxClasses->currentData().toInt();
	qf::core::sql::QueryBuilder qb;
	qb.select2("classdefs", "classId, relayStartNumber").from("classdefs");
	if(class_id > 0)
		qb.where("classId=" + QString::number(class_id));
	qf::core::sql::Query q;
	q.execThrow(qb.toString());
	while(q.next()) {
		class_id = q.value("classId").toInt();
		int start_num = q.value("relayStartNumber").toInt();
		QVector<int> ids;
		QVector<int> nums;
		qf::core::sql::Query q2;
		q2.execThrow("SELECT id FROM relays WHERE classId=" + QString::number(class_id) + " ORDER BY club, name");
		while(q2.next()) {
			ids << q2.value(0).toInt();
			nums << start_num++;
		}
		if(method == 0)
			std::random_shuffle(nums.begin(), nums.end());
		for (int i = 0; i < ids.count(); ++i) {
			q2.execThrow("UPDATE relays SET number=" + QString::number(nums[i]) + " WHERE id=" + QString::number(ids[i]));
		}
	}
	transaction.commit();
	reload();
}

QVariant RelaysWidget::startListByClassesTableData(const QString &class_filter)
{
	qfLogFuncFrame() << class_filter;
	qf::core::model::SqlTableModel model;
	qf::core::model::SqlTableModel model2;
	{
		qf::core::sql::QueryBuilder qb;
		qb.select2("classes", "id, name")
			.from("classes")
			.orderBy("classes.name");//.limit(1);
		if(!class_filter.isEmpty()) {
			qb.where(class_filter);
		}
		model.setQueryBuilder(qb, true);
	}
	//console.info("currentStageTable query:", reportModel.effectiveQuery());
	model.reload();
	qf::core::utils::TreeTable tt = model.toTreeTable();
	tt.setValue("event", getPlugin<EventPlugin>()->eventConfig()->value("event"));
	tt.setValue("stageStart", getPlugin<EventPlugin>()->stageStartDateTime(1));
	{
		qf::core::sql::QueryBuilder qb;
		qb.select2("relays", "id, name, number")
				.select2("clubs", "name")
				.select("COALESCE(relays.club, '') || ' ' || COALESCE(relays.name, '') AS relayName")
				.from("relays")
				.join("relays.club", "clubs.abbr")
				.where("relays.classId={{class_id}}")
				.orderBy("relays.number, relayName");
		model.setQueryBuilder(qb, true);
	}
	{
		qf::core::sql::QueryBuilder qb;
		qb.select2("competitors", "registration")
			.select("COALESCE(competitors.lastName, '') || ' ' || COALESCE(competitors.firstName, '') AS competitorName")
			.select2("runs", "leg, siId")
			.from("runs")
			.join("runs.competitorId", "competitors.id")
			.join("runs.relayId", "relays.id")
			.where("runs.relayId={{relay_id}}")
			.where("runs.isRunning")
			.orderBy("runs.leg");
		model2.setQueryBuilder(qb, true);
	}
	for(int i=0; i<tt.rowCount(); i++) {
		int class_id = tt.row(i).value("classes.id").toInt();
		//console.debug("class id:", class_id);
		QVariantMap qm;
		qm["class_id"] = class_id;
		model.setQueryParameters(qm);
		model.reload();
		qf::core::utils::TreeTable tt2 = model.toTreeTable();
		for (int j = 0; j < tt2.rowCount(); ++j) {
			int relay_id = tt2.row(j).value("relays.id").toInt();
			QVariantMap qm2;
			qm2["relay_id"] = relay_id;
			model2.setQueryParameters(qm2);
			model2.reload();
			qf::core::utils::TreeTable tt3 = model2.toTreeTable();
			tt2.appendTable(j, tt3);
		}
		tt.appendTable(i, tt2);
	}
	//qfInfo() << tt.toString();
	return tt.toVariant();
}

QVariant RelaysWidget::startListByClubsTableData()
{
	qfLogFuncFrame();
	qf::core::model::SqlTableModel model;
	qf::core::model::SqlTableModel model2;
	{
		qf::core::sql::QueryBuilder qb1;
		qb1.select("relays.club")
				.from("relays")
				.groupBy("club")
				.orderBy("club")
				.as("relay_clubs");
		qf::core::sql::QueryBuilder qb;
		qb.select2("relay_clubs", "club")
				.select2("clubs", "name")
				.from(qb1)
				.join("relay_clubs.club", "clubs.abbr")
				.orderBy("club");//.limit(1);
		model.setQueryBuilder(qb, true);
	}
	//console.info("currentStageTable query:", reportModel.effectiveQuery());
	model.reload();
	qf::core::utils::TreeTable tt = model.toTreeTable();
	tt.setValue("event", getPlugin<EventPlugin>()->eventConfig()->value("event"));
	tt.setValue("stageStart", getPlugin<EventPlugin>()->stageStartDateTime(1));
	{
		qf::core::sql::QueryBuilder qb;
		qb.select2("relays", "id, name, number")
				.select2("classes", "name")
				.select("COALESCE(relays.club, '') || ' ' || COALESCE(relays.name, '') AS relayName")
				.from("relays")
				.join("relays.classId", "classes.id")
				.where("relays.club='{{club}}'")
				.orderBy("classes.name, relayName");
		model.setQueryBuilder(qb, true);
	}
	{
		qf::core::sql::QueryBuilder qb;
		qb.select2("competitors", "registration")
			.select("COALESCE(competitors.lastName, '') || ' ' || COALESCE(competitors.firstName, '') AS competitorName")
			.select2("runs", "leg, siId")
			.from("runs")
			.join("runs.competitorId", "competitors.id")
			.join("runs.relayId", "relays.id")
			.where("runs.relayId={{relay_id}}")
			.where("runs.isRunning")
			.orderBy("runs.leg");
		model2.setQueryBuilder(qb, true);
	}
	for(int i=0; i<tt.rowCount(); i++) {
		QString club = tt.row(i).value("club").toString();
		//console.debug("class id:", class_id);
		QVariantMap qm;
		qm["club"] = club;
		model.setQueryParameters(qm);
		model.reload();
		qf::core::utils::TreeTable tt2 = model.toTreeTable();
		for (int j = 0; j < tt2.rowCount(); ++j) {
			int relay_id = tt2.row(j).value("relays.id").toInt();
			QVariantMap qm2;
			qm2["relay_id"] = relay_id;
			model2.setQueryParameters(qm2);
			model2.reload();
			qf::core::utils::TreeTable tt3 = model2.toTreeTable();
			tt2.appendTable(j, tt3);
		}
		tt.appendTable(i, tt2);
	}
	//console.debug(tt.toString());
	return tt.toVariant();
}

void RelaysWidget::print_start_list_classes()
{
	quickevent::gui::ReportOptionsDialog dlg(this);
	dlg.setPersistentSettingsId("relaysStartReportOptions");
	//dlg.setClassNamesFilter(class_names);
	if(!dlg.exec())
		return;
	QVariantMap props = dlg.reportProperties();
	QVariant td = startListByClassesTableData(dlg.sqlWhereExpression());
	qf::qmlwidgets::reports::ReportViewWidget::showReport(this,
														  getPlugin<RelaysPlugin>()->qmlDir() + "/startList_classes.qml"
														  , td
														  , tr("Start list by classes")
														  , "printStartList"
														  , props
														  );
}

void RelaysWidget::print_start_list_clubs()
{
	quickevent::gui::ReportOptionsDialog dlg(this);
	dlg.setPersistentSettingsId("relaysStartReportOptions");
	dlg.loadPersistentSettings();
	dlg.setClassFilterVisible(false);
	if(!dlg.exec())
		return;
	QVariantMap props = dlg.reportProperties();
	QVariant td = startListByClubsTableData();
	qf::qmlwidgets::reports::ReportViewWidget::showReport(this,
														  getPlugin<RelaysPlugin>()->qmlDir() + "/startList_clubs.qml"
														  , td
														  , tr("Start list by clubs")
														  , "printStartList"
														  , props
														  );
}

void RelaysWidget::print_results_nlegs()
{
	qfLogFuncFrame();
	quickevent::gui::ReportOptionsDialog::Options default_opts;
	default_opts.setLegsCount(1);
	default_opts.setResultExcludeDisq(true);

	quickevent::gui::ReportOptionsDialog dlg(this);
	dlg.setLegsOptionVisible(true);
	dlg.setResultOptionsVisible(true);
	dlg.setPersistentSettingsId("relaysResultsNLegsReportOptions");
	dlg.loadPersistentSettings(default_opts);
	if(!dlg.exec())
		return;
	QVariantMap props = dlg.reportProperties();
	quickevent::gui::ReportOptionsDialog::Options opts = dlg.options();
	//qfDebug() << opts;
	qfDebug() << "opts.resultNumPlaces:" << opts.resultNumPlaces();
	auto td = getPlugin<RelaysPlugin>()->nLegsResultsTable(dlg.sqlWhereExpression(), opts.legsCount(), opts.resultNumPlaces(), opts.isResultExcludeDisq());
	qf::qmlwidgets::reports::ReportViewWidget::showReport(this,
														  getPlugin<RelaysPlugin>()->qmlDir() + "/results.qml"
														  , td.toVariant()
														  , tr("Results")
														  , "relaysResults"
														  , props
														  );
}

void RelaysWidget::print_results_overal()
{
	qfLogFuncFrame();
	quickevent::gui::ReportOptionsDialog dlg(this);
	dlg.setLegsOptionVisible(false);
	dlg.setResultOptionsVisible(true);
	dlg.setPersistentSettingsId("relaysResultsOverallReportOptions");
	dlg.loadPersistentSettings();
	if(!dlg.exec())
		return;
	QVariantMap props = dlg.reportProperties();
	quickevent::gui::ReportOptionsDialog::Options opts = dlg.options();
	//qfDebug() << opts;
	qfDebug() << "opts.resultNumPlaces:" << opts.resultNumPlaces();
	auto td = getPlugin<RelaysPlugin>()->nLegsResultsTable(dlg.sqlWhereExpression(), 999, opts.resultNumPlaces(), opts.isResultExcludeDisq());
	qf::qmlwidgets::reports::ReportViewWidget::showReport(this,
														  getPlugin<RelaysPlugin>()->qmlDir() + "/results.qml"
														  , td.toVariant()
														  , tr("Results")
														  , "relaysResults"
														  , props
														  );
}

void RelaysWidget::print_results_overal_condensed()
{
	qfLogFuncFrame();
	quickevent::gui::ReportOptionsDialog dlg(this);
	dlg.setLegsOptionVisible(false);
	dlg.setResultOptionsVisible(true);
	dlg.setPersistentSettingsId("relaysResultsOverallCondensedReportOptions");
	dlg.loadPersistentSettings();
	if(!dlg.exec())
		return;
	QVariantMap props = dlg.reportProperties();
	quickevent::gui::ReportOptionsDialog::Options opts = dlg.options();
	//qfDebug() << opts;
	qfDebug() << "opts.resultNumPlaces:" << opts.resultNumPlaces();
	auto td = getPlugin<RelaysPlugin>()->nLegsResultsTable(dlg.sqlWhereExpression(), 999, opts.resultNumPlaces(), opts.isResultExcludeDisq());
	qf::qmlwidgets::reports::ReportViewWidget::showReport(this,
														  getPlugin<RelaysPlugin>()->qmlDir() + "/results_condensed.qml"
														  , td.toVariant()
														  , tr("Results")
														  , "relaysResults"
														  , props
														  );
}

static void append_list(QVariantList &lst, const QVariantList &new_lst)
{
	lst.insert(lst.count(), new_lst);
}

void RelaysWidget::export_results_iofxml3()
{
	qfLogFuncFrame();
	QString fn = "relays-results-iof-3.0.xml";
	QString ext = ".xml";
	fn = qfd::FileDialog::getSaveFileName(this, tr("Save as %1").arg(ext.mid(1).toUpper()), fn, '*' + ext);
	if(!fn.isEmpty()) {
		if(!fn.endsWith(ext, Qt::CaseInsensitive))
			fn += ext;
	}
	if(fn.isEmpty())
		return;	
	QFile out_file(fn);
	if(!out_file.open(QIODevice::WriteOnly)) {
		qfError() << "Cannot open file" << out_file.fileName() << "for writing.";
		return;
	}

	int progress_count = 0;
	int progress_val = 0;
	{
		auto q = qfs::Query::fromExec("SELECT COUNT(*) FROM classes");
		if(q.next())
			progress_count = q.value(0).toInt() + 1;
	}

	QProgressDialog progress(tr("Exporting result file..."), tr("Abort"), 0, progress_count + 1, this);
	progress.setWindowModality(Qt::WindowModal);

	QDateTime start00 = getPlugin<EventPlugin>()->stageStartDateTime(1);
	qfDebug() << "creating table";
	//auto tt_classes = getPlugin<RelaysPlugin>()->nLegsResultsTable("classes.name='D105'", 999, 999999, false);
	auto tt_classes = getPlugin<RelaysPlugin>()->nLegsResultsTable(QString(), 999, 999999, false);
	progress.setValue(++progress_val);
	QVariantList result_list{
		"ResultList",
		QVariantMap{
			{"xmlns", "http://www.orienteering.org/datastandard/3.0"},
			{"status", "Complete"},
			{"iofVersion", "3.0"},
			{"creator", "QuickEvent"},
			{"createTime", datetime_to_string(QDateTime::currentDateTime())},
		}
	};
	{
		QVariantList event_lst{"Event"};
		QVariantMap event = tt_classes.value("event").toMap();
		event_lst.insert(event_lst.count(), QVariantList{"Id", QVariantMap{{"type", "ORIS"}}, event.value("importId")});
		event_lst.insert(event_lst.count(), QVariantList{"Name", event.value("name")});
		event_lst.insert(event_lst.count(), QVariantList{"StartTime",
				   QVariantList{"Date", event.value("date")},
				   QVariantList{"Time", event.value("time")}
		});
		event_lst.insert(event_lst.count(),
			QVariantList{"Official",
				QVariantMap{{"type", "director"}},
				QVariantList{"Person",
					QVariantList{"Name",
						QVariantList{"Family", event.value("director").toString().section(' ', 1, 1)},
						QVariantList{"Given", event.value("director").toString().section(' ', 0, 0)},
					}
				},
			}
		);
		event_lst.insert(event_lst.count(),
			QVariantList{"Official",
				QVariantMap{{"type", "mainReferee"}},
				QVariantList{"Person",
					QVariantList{"Name",
						QVariantList{"Family", event.value("mainReferee").toString().section(' ', 1, 1)},
						QVariantList{"Given", event.value("mainReferee").toString().section(' ', 0, 0)},
					}
				},
			}
		);
		result_list.insert(result_list.count(), event_lst);
	}
	for(int i=0; i<tt_classes.rowCount(); i++) {
		QVariantList class_result{"ClassResult"};
		const qf::core::utils::TreeTableRow tt_classes_row = tt_classes.row(i);
		QF_TIME_SCOPE("exporting class: " + tt_classes_row.value(QStringLiteral("className")).toString());
		append_list(class_result,
			QVariantList{"Class",
				QVariantList{"Name", tt_classes_row.value(QStringLiteral("className")) },
			}
		);
		qf::core::utils::TreeTable tt_teams = tt_classes_row.table();
		for(int j=0; j<tt_teams.rowCount(); j++) {
			QVariantList team_result{"TeamResult"};
			const qf::core::utils::TreeTableRow tt_teams_row = tt_teams.row(j);
			QF_TIME_SCOPE("exporting team: " + tt_teams_row.value(QStringLiteral("name")).toString());
			append_list(team_result,
				QVariantList{"Name", tt_teams_row.value(QStringLiteral("name")) }
			);
			int relay_number = tt_teams_row.value(QStringLiteral("relayNumber")).toInt();
			append_list(team_result,
				QVariantList{"BibNumber", relay_number }
			);

			qf::core::utils::TreeTable tt_legs = tt_teams_row.table();
			for (int k = 0; k < tt_legs.rowCount(); ++k) {
				int leg = k + 1;
				QF_TIME_SCOPE("exporting leg: " + QString::number(leg));
				const qf::core::utils::TreeTableRow tt_leg_row = tt_legs.row(k);
				QVariantList member_result{"TeamMemberResult"};
				append_list(member_result,
							QVariantList{"Person",
								QVariantList{"Id", QVariantMap{{"type", "CZE"}}, tt_leg_row.value(QStringLiteral("registration"))},
								QVariantList{"Name",
								   QVariantList{"Family", tt_leg_row.value(QStringLiteral("lastName"))},
								   QVariantList{"Given", tt_leg_row.value(QStringLiteral("firstName"))},
								}
							} );
				QVariantList person_result{"Result"};
				append_list(person_result, QVariantList{"Leg", k+1 } );
				append_list(person_result, QVariantList{"BibNumber", QString::number(relay_number) + '.' + QString::number(k+1)});
				int run_id = tt_leg_row.value(QStringLiteral("runId")).toInt();
				int stime = 0, ftime = 0, time_msec = 0;
				if(run_id > 0) {
					qfs::QueryBuilder qb;
					qb.select2("runs", "startTimeMs, finishTimeMs, timeMs")
							.from("runs").where("id=" + QString::number(run_id));
					qfs::Query q;
					q.execThrow(qb.toString());
					if(q.next()) {
						stime = q.value(0).toInt();
						ftime = q.value(1).toInt();
						time_msec = q.value(2).toInt();
					}
					else {
						qfWarning() << "Cannot load run for id:" << run_id;
					}
				}
				append_list(person_result, QVariantList{"StartTime", datetime_to_string(start00.addMSecs(stime))});
				append_list(person_result, QVariantList{"FinishTime", datetime_to_string(start00.addMSecs(ftime))});
				append_list(person_result, QVariantList{"Time", time_msec / 1000});
				append_list(person_result, QVariantList{"Position", QVariantMap{{"type", "Leg"}}, tt_leg_row.value(QStringLiteral("pos"))});
				// MISSING position course append_list(person_result, QVariantList{"Position", QVariantMap{{"type", "course"}}, tt_laps_row.value(QStringLiteral("pos"))});
				append_list(person_result, QVariantList{"Status", tt_leg_row.value(QStringLiteral("status"))});
				QVariantList overall_result{"OverallResult"};
				{
					append_list(overall_result, QVariantList{"Time", tt_leg_row.value(QStringLiteral("stime")).toInt() / 1000});
					append_list(overall_result, QVariantList{"Position", tt_leg_row.value(QStringLiteral("spos"))});
					append_list(overall_result, QVariantList{"Status", tt_leg_row.value(QStringLiteral("sstatus"))});
					// MISSING TimeBehind
				}
				append_list(person_result, overall_result);
				int course_id = getPlugin<RunsPlugin>()->courseForRelay(relay_number, leg);
				{
					QF_TIME_SCOPE("exporting course: " + QString::number(course_id));
					QVariantList course{"Course"};
					append_list(course, QVariantList{"Id", course_id});
					{
						qfs::QueryBuilder qb;
						qb.select2("courses", "name, length, climb")
								.from("courses").where("id=" + QString::number(course_id));
						qfs::Query q;
						q.execThrow(qb.toString());
						if(q.next()) {
							append_list(course, QVariantList{"Name", q.value(0)});
							append_list(course, QVariantList{"Length", q.value(1)});
							append_list(course, QVariantList{"Climb", q.value(2)});
						}
						else {
							qfWarning() << "Cannot load course for id:" << course_id;
						}
					}
					append_list(person_result, course);
				}
				{
					QF_TIME_SCOPE("exporting laps");
					qf::core::sql::QueryBuilder qb;
					qb.select2("runlaps", "position, stpTimeMs")
							.from("runlaps").where("runId=" + QString::number(run_id))
							.where("code >= " + QString::number(quickevent::core::CodeDef::PUNCH_CODE_MIN))
							.where("code <= " + QString::number(quickevent::core::CodeDef::PUNCH_CODE_MAX))
							.orderBy("position") ;
					//qfInfo() << qb.toString();
					auto q = qf::core::sql::Query::fromExec(qb.toString());
					quickevent::core::CourseDef csd = getPlugin<RunsPlugin>()->courseForCourseId(course_id);
					QVariantList codes = csd.codes();
					int sql_pos = -1;
					for (int ix = 0; ix < codes.count(); ++ix) {
						quickevent::core::CodeDef cd(codes[ix].toMap());
						int pos = ix + 1;
						if(sql_pos < 0) {
							if(q.next()) {
								sql_pos = q.value(0).toInt();
							}
						}
						int time = 0;
						if(pos == sql_pos) {
							sql_pos = -1;
							time = q.value(1).toInt();
						}
						QVariantList split{QStringLiteral("SplitTime")};
						append_list(split, QVariantList{"ControlCode", cd.code()});
						if(time == 0)
							split.insert(1, QVariantMap{ {QStringLiteral("status"), QStringLiteral("Missing")} });
						else
							append_list(split, QVariantList{"Time", time / 1000});

						append_list(person_result, split);
					}
				}
				append_list(member_result, person_result);
				append_list(team_result, member_result);
			}
			append_list(class_result, team_result);
		}
		progress.setValue(++progress_val);
		if (progress.wasCanceled())
			break;
		append_list(result_list, class_result);
	}
	qf::core::utils::HtmlUtils::FromXmlListOptions opts;
	opts.setDocumentTitle(tr("Relays IOF-XML 3.0 results"));
	QString str = qf::core::utils::HtmlUtils::fromXmlList(result_list, opts);
	out_file.write(str.toUtf8());
	qfInfo() << "exported:" << out_file.fileName();
	progress.setValue(progress_count);
}


