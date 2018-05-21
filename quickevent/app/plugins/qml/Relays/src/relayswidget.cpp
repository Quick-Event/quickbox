#include "relayswidget.h"
#include "ui_relayswidget.h"
#include "relaywidget.h"
#include "thispartwidget.h"
#include "lentcardswidget.h"

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

static Event::EventPlugin* eventPlugin()
{
	qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
	auto *plugin = qobject_cast<Event::EventPlugin*>(fwk->plugin("Event"));
	QF_ASSERT_EX(plugin != nullptr, "Bad Event plugin!");
	return plugin;
}

static Relays::RelaysPlugin* competitorsPlugin()
{
	qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
	auto *plugin = qobject_cast<Relays::RelaysPlugin*>(fwk->plugin("Relays"));
	QF_ASSERT_EX(plugin != nullptr, "Bad Relays plugin!");
	return plugin;
}

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
	m->addColumn("id").setReadOnly(true);
	m->addColumn("classes.name", tr("Class"));
	m->addColumn("competitors.startNumber", tr("SN", "start number")).setToolTip(tr("Start number"));
	m->addColumn("competitorName", tr("Name"));
	m->addColumn("registration", tr("Reg"));
	m->addColumn("siId", tr("SI")).setReadOnly(true).setCastType(qMetaTypeId<quickevent::si::SiId>());
	m->addColumn("ranking", tr("Ranking"));
	m->addColumn("note", tr("Note"));
	ui->tblRelays->setTableModel(m);
	m_competitorsModel = m;

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
	main_tb->addSeparator();
	{
		m_cbxEditRelayOnPunch = new QCheckBox(tr("Edit on punch"));
		m_cbxEditRelayOnPunch->setToolTip(tr("Edit or insert competitor on card insert into station."));
		main_tb->addWidget(m_cbxEditRelayOnPunch);
	}

	qf::qmlwidgets::Action *act_print = part_widget->menuBar()->actionForPath("print");
	act_print->setText(tr("&Print"));

	qf::qmlwidgets::Action *act_cards = part_widget->menuBar()->actionForPath("cards");
	act_cards->setText(tr("&Cards"));
	{
		qf::qmlwidgets::Action *a = new qf::qmlwidgets::Action("lentCards", tr("Lent cards"));
		act_cards->addActionInto(a);
		connect(a, &qf::qmlwidgets::Action::triggered, [this]() {
			qf::qmlwidgets::dialogs::Dialog dlg(this);
			auto *w = new LentCardsWidget();
			dlg.setCentralWidget(w);
			dlg.exec();
		});
	}

}

void RelaysWidget::lazyInit()
{
}

void RelaysWidget::reset()
{
	if(!eventPlugin()->isEventOpen()) {
		m_competitorsModel->clearRows();
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
	qb.select2("competitors", "*")
			.select2("classes", "name")
			.select("COALESCE(lastName, '') || ' ' || COALESCE(firstName, '') AS competitorName")
			.from("competitors")
			.join("competitors.classId", "classes.id")
			.orderBy("competitors.id");//.limit(10);
	int class_id = m_cbxClasses->currentData().toInt();
	if(class_id > 0) {
		qb.where("competitors.classId=" + QString::number(class_id));
	}
	m_competitorsModel->setQueryBuilder(qb, false);
	m_competitorsModel->reload();
}

void RelaysWidget::editRelay_helper(const QVariant &id, int mode, int siid)
{
	qfLogFuncFrame() << "id:" << id << "mode:" << mode;
	m_cbxEditRelayOnPunch->setEnabled(false);
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
		if(siid == 0) {
			int class_id = m_cbxClasses->currentData().toInt();
			doc->setValue("competitors.classId", class_id);
		}
		else {
			w->loadFromRegistrations(siid);
		}
	}
	connect(doc, &Relays:: RelayDocument::saved, ui->tblRelays, &qf::qmlwidgets::TableView::rowExternallySaved, Qt::QueuedConnection);
	connect(doc, &Relays:: RelayDocument::saved, competitorsPlugin(), &Relays::RelaysPlugin::competitorEdited, Qt::QueuedConnection);
	bool ok = dlg.exec();
	m_cbxEditRelayOnPunch->setEnabled(true);
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
				if(qfd::MessageBox::askYesNo(this, tr("Confirm deletion of %1 competitors.").arg(n), false)) {
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
	if(m_cbxEditRelayOnPunch->isEnabled() && m_cbxEditRelayOnPunch->isChecked() && domain == QLatin1String(Event::EventPlugin::DBEVENT_PUNCH_RECEIVED)) {
		quickevent::si::PunchRecord punch(data.toMap());
		int siid = punch.siid();
		if(siid > 0 && punch.marking() == quickevent::si::PunchRecord::MARKING_ENTRIES) {
			editRelayOnPunch(siid);
		}
	}
}

void RelaysWidget::editRelayOnPunch(int siid)
{
	qfs::Query q;
	q.exec("SELECT id FROM competitors WHERE siId=" + QString::number(siid), qfc::Exception::Throw);
	if(q.next()) {
		int competitor_id = q.value(0).toInt();
		if(competitor_id > 0) {
			editRelay_helper(competitor_id, qfm::DataDocument::ModeEdit, 0);
		}
	}
	else {
		editRelay_helper(QVariant(), qfm::DataDocument::ModeInsert, siid);
	}
}
