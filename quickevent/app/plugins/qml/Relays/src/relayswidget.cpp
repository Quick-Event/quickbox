#include "relayswidget.h"
#include "ui_relayswidget.h"
#include "relaywidget.h"
#include "thispartwidget.h"

#include "Relays/relaydocument.h"
#include "Relays/relaysplugin.h"

#include "Event/eventplugin.h"

#include <quickevent/si/siid.h>
#include <quickevent/si/punchrecord.h>

#include <qf/qmlwidgets/dialogs/dialog.h>
#include <qf/qmlwidgets/dialogs/messagebox.h>
#include <qf/qmlwidgets/framework/mainwindow.h>
#include <qf/qmlwidgets/framework/plugin.h>
#include <qf/qmlwidgets/toolbar.h>
#include <qf/qmlwidgets/combobox.h>
#include <qf/qmlwidgets/action.h>
#include <qf/qmlwidgets/menubar.h>
#include <qf/qmlwidgets/dialogbuttonbox.h>

#include <qf/core/model/sqltablemodel.h>
#include <qf/core/sql/querybuilder.h>
#include <qf/core/sql/transaction.h>
#include <qf/core/assert.h>

#include <QCheckBox>
#include <QLabel>
#include <QPushButton>
#include <QTimer>

namespace qfs = qf::core::sql;
namespace qfw = qf::qmlwidgets;
namespace qff = qf::qmlwidgets::framework;
namespace qfd = qf::qmlwidgets::dialogs;
namespace qfc = qf::core;
namespace qfm = qf::core::model;

namespace {
Event::EventPlugin* eventPlugin()
{
	qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
	auto *plugin = qobject_cast<Event::EventPlugin*>(fwk->plugin("Event"));
	QF_ASSERT_EX(plugin != nullptr, "Bad Event plugin!");
	return plugin;
}

Relays::RelaysPlugin* competitorsPlugin()
{
	qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
	auto *plugin = qobject_cast<Relays::RelaysPlugin*>(fwk->plugin("Relays"));
	QF_ASSERT_EX(plugin != nullptr, "Bad Relays plugin!");
	return plugin;
}

enum Columns {
	col_relays_id = 0,
	col_classes_name,
	//col_relays_club,
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
	ui->tblRelays->setPersistentSettingsId("tblRelays");
	ui->tblRelays->setRowEditorMode(qfw::TableView::EditRowsMixed);
	ui->tblRelays->setInlineEditSaveStrategy(qfw::TableView::OnEditedValueCommit);
	qfm::SqlTableModel *m = new qfm::SqlTableModel(this);
	using CD = qfm::TableModel::ColumnDefinition;
	m->clearColumns(col_COUNT);
	m->setColumn(col_relays_id, CD("id").setReadOnly(true));
	m->setColumn(col_classes_name, CD("classes.name", tr("Class")).setReadOnly(true));
	//m->setColumn(col_relays_club, CD("club", tr("Club")));
	m->setColumn(col_relays_name, CD("name", tr("Name")));
	m->setColumn(col_relays_number, CD("number", tr("Number")));
	m->setColumn(col_relays_note, CD("note", tr("Note")));
	ui->tblRelays->setTableModel(m);
	m_tblModel = m;

	//connect(ui->tblRelays, SIGNAL(editRowInExternalEditor(QVariant,int)), this, SLOT(edit Relay(QVariant,int)), Qt::QueuedConnection);
	connect(ui->tblRelays, &qfw::TableView::editRowInExternalEditor, this, &RelaysWidget::editRelay, Qt::QueuedConnection);
	connect(ui->tblRelays, &qfw::TableView::editSelectedRowsInExternalEditor, this, &RelaysWidget::editRelays, Qt::QueuedConnection);

	connect(competitorsPlugin(), &Relays::RelaysPlugin::dbEventNotify, this, &RelaysWidget::onDbEventNotify);

	QMetaObject::invokeMethod(this, "lazyInit", Qt::QueuedConnection);
}

RelaysWidget::~RelaysWidget()
{
	delete ui;
}

void RelaysWidget::settleDownInPartWidget(ThisPartWidget *part_widget)
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
			m_cbxClasses->setMinimumWidth(fontMetrics().width('X') * 10);
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
	qf::qmlwidgets::Action *act_print = part_widget->menuBar()->actionForPath("print");
	act_print->setText(tr("&Print"));
}

void RelaysWidget::lazyInit()
{
}

void RelaysWidget::reset()
{
	if(!eventPlugin()->isEventOpen()) {
		m_tblModel->clearRows();
		return;
	}
	{
		m_cbxClasses->blockSignals(true);
		m_cbxClasses->loadItems(true);
		m_cbxClasses->insertItem(0, tr("--- all ---"), 0);
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
	auto *w = new  RelayWidget();
	w->setWindowTitle(tr("Edit  Relay"));
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
	connect(doc, &Relays:: RelayDocument::saved, competitorsPlugin(), &Relays::RelaysPlugin::competitorEdited, Qt::QueuedConnection);
	bool ok = dlg.exec();
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
		if(qfd::MessageBox::askYesNo(this, tr("Realy delete all the selected competitors? This action cannot be reverted."), false)) {
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

void RelaysWidget::onDbEventNotify(const QString &domain, int connection_id, const QVariant &data)
{
	Q_UNUSED(connection_id)
	qfLogFuncFrame() << "domain:" << domain << "payload:" << data;
}

