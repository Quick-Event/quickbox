#include "relayswidget.h"
#include "ui_relayswidget.h"
#include "relaywidget.h"

#include "relaydocument.h"
#include "relaysplugin.h"

#include <plugins/Event/src/eventplugin.h>
#include <plugins/Runs/src/runsplugin.h>

#include "partwidget.h"
#include <quickevent/gui/reportoptionsdialog.h>

#include <quickevent/core/si/siid.h>
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

#include <qf/core/sql/query.h>
#include <qf/core/model/sqltablemodel.h>
#include <qf/core/sql/querybuilder.h>
#include <qf/core/sql/transaction.h>
#include <qf/core/assert.h>
#include <qf/core/utils/treetable.h>
#include <QCheckBox>
#include <QInputDialog>
#include <QLabel>
#include <QProgressDialog>
#include <QPushButton>
#include <QTimer>

//#define QF_TIMESCOPE_ENABLED
#include <qf/core/utils/timescope.h>
#include <random>

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

	connect(ui->tblRelays, &qfw::TableView::editRowInExternalEditor, this, &RelaysWidget::editRelay, Qt::QueuedConnection);
	connect(ui->tblRelays, &qfw::TableView::editSelectedRowsInExternalEditor, this, &RelaysWidget::editRelays, Qt::QueuedConnection);

	//connect(getPlugin<EventPlugin>(), &Relays::RelaysPlugin::dbEventNotify, this, &RelaysWidget::onDbEventNotify);

	QMetaObject::invokeMethod(this, "lazyInit", Qt::QueuedConnection);
}

RelaysWidget::~RelaysWidget()
{
	delete ui;
}

void RelaysWidget::settleDownInPartWidget(::PartWidget *part_widget)
{
	connect(part_widget, &::PartWidget::resetPartRequest, this, &RelaysWidget::reset);
	connect(part_widget, &::PartWidget::reloadPartRequest, this, &RelaysWidget::reload);

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
	qfw::Action *a_export_startlist = a_export->addMenuInto("export", tr("&Start list"));
	{
		qfw::Action *a = new qfw::Action("exportStartListIofXml3", tr("IOF-XML 3.0"));
		a_export_startlist->addActionInto(a);
		connect(a, &qfw::Action::triggered, this, &RelaysWidget::export_start_list_iofxml3);
	}
	qfw::Action *a_export_results = a_export->addMenuInto("export", tr("&Results"));
	{
		qfw::Action *a = new qfw::Action("exportResultsIofXml3", tr("IOF-XML 3.0"));
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
		connect(m_cbxClasses, &qf::qmlwidgets::ForeignKeyComboBox::currentDataChanged, this, &RelaysWidget::reload, Qt::UniqueConnection);
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
		if(method == 0) {
			std::random_device rd;
			std::mt19937 g(rd());
			std::shuffle(nums.begin(), nums.end(), g);
		}
		for (int i = 0; i < ids.count(); ++i) {
			q2.execThrow("UPDATE relays SET number=" + QString::number(nums[i]) + " WHERE id=" + QString::number(ids[i]));
		}
	}
	transaction.commit();
	reload();
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
	QVariant td = getPlugin<RelaysPlugin>()->startListByClassesTableData(dlg.sqlWhereExpression());
	qf::qmlwidgets::reports::ReportViewWidget::showReport(this
														  , getPlugin<RelaysPlugin>()->findReportFile("startList_classes.qml")
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
														  getPlugin<RelaysPlugin>()->findReportFile("startList_clubs.qml")
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
														  getPlugin<RelaysPlugin>()->findReportFile("results.qml")
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
														  getPlugin<RelaysPlugin>()->findReportFile("results.qml")
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
														  getPlugin<RelaysPlugin>()->findReportFile("results_condensed.qml")
														  , td.toVariant()
														  , tr("Results")
														  , "relaysResults"
														  , props
														  );
}


void RelaysWidget::save_xml_file(QString str, QString fn) {
	qfLogFuncFrame();
	QString ext = ".xml";
	fn = qf::qmlwidgets::dialogs::FileDialog::getSaveFileName(this, tr("Save as %1").arg(ext.mid(1).toUpper()), fn, '*' + ext);
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

	out_file.write(str.toUtf8());
	qfInfo() << "exported:" << out_file.fileName();
}


void RelaysWidget::export_start_list_iofxml3() {

	QString fn = getPlugin<EventPlugin>()->eventName() + ".startlist.iof30.xml";
	save_xml_file(getPlugin<RelaysPlugin>()->startListIofXml30(), fn);
}

void RelaysWidget::export_results_iofxml3() {
	QString fn = getPlugin<EventPlugin>()->eventName() + ".results.iof30.xml";
	save_xml_file(getPlugin<RelaysPlugin>()->resultsIofXml30(), fn);
}
