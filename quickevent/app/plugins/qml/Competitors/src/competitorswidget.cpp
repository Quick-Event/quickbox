#include "competitorswidget.h"
#include "ui_competitorswidget.h"
#include "competitorwidget.h"
#include "thispartwidget.h"
#include "lentcardswidget.h"

#include "Competitors/competitordocument.h"
#include "Competitors/competitorsplugin.h"

#include "Event/eventplugin.h"

#include <quickevent/core/si/siid.h>
#include <quickevent/core/si/punchrecord.h>

#include <qf/qmlwidgets/dialogs/dialog.h>
#include <qf/qmlwidgets/dialogs/getiteminputdialog.h>
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

static Competitors::CompetitorsPlugin* competitorsPlugin()
{
	qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
	auto *plugin = qobject_cast<Competitors::CompetitorsPlugin*>(fwk->plugin("Competitors"));
	QF_ASSERT_EX(plugin != nullptr, "Bad Competitors plugin!");
	return plugin;
}

CompetitorsWidget::CompetitorsWidget(QWidget *parent) :
	Super(parent),
	ui(new Ui::CompetitorsWidget)
{
	ui->setupUi(this);

	ui->tblCompetitorsToolBar->setTableView(ui->tblCompetitors);

	ui->tblCompetitors->setCloneRowEnabled(false);
	ui->tblCompetitors->setPersistentSettingsId("tblCompetitors");
	ui->tblCompetitors->setRowEditorMode(qfw::TableView::EditRowsMixed);
	ui->tblCompetitors->setInlineEditSaveStrategy(qfw::TableView::OnEditedValueCommit);
	qfm::SqlTableModel *m = new qfm::SqlTableModel(this);
	m->addColumn("id").setReadOnly(true);
	m->addColumn("classes.name", tr("Class"));
	m->addColumn("competitors.startNumber", tr("SN", "start number")).setToolTip(tr("Start number"));
	m->addColumn("competitorName", tr("Name"));
	m->addColumn("registration", tr("Reg"));
	m->addColumn("siId", tr("SI")).setReadOnly(true).setCastType(qMetaTypeId<quickevent::core::si::SiId>());
	m->addColumn("ranking", tr("Ranking"));
	m->addColumn("note", tr("Note"));
	ui->tblCompetitors->setTableModel(m);
	m_competitorsModel = m;

	//connect(ui->tblCompetitors, SIGNAL(editRowInExternalEditor(QVariant,int)), this, SLOT(editCompetitor(QVariant,int)), Qt::QueuedConnection);
	connect(ui->tblCompetitors, &qfw::TableView::editRowInExternalEditor, this, &CompetitorsWidget::editCompetitor, Qt::QueuedConnection);
	connect(ui->tblCompetitors, &qfw::TableView::editSelectedRowsInExternalEditor, this, &CompetitorsWidget::editCompetitors, Qt::QueuedConnection);

	ui->tblCompetitors->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(ui->tblCompetitors, &qfw::TableView::customContextMenuRequested, this, &CompetitorsWidget::onCustomContextMenuRequest);

	connect(competitorsPlugin(), &Competitors::CompetitorsPlugin::dbEventNotify, this, &CompetitorsWidget::onDbEventNotify);

	QMetaObject::invokeMethod(this, "lazyInit", Qt::QueuedConnection);
}

CompetitorsWidget::~CompetitorsWidget()
{
	delete ui;
}

void CompetitorsWidget::settleDownInPartWidget(ThisPartWidget *part_widget)
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
		m_cbxEditCompetitorOnPunch = new QCheckBox(tr("Edit on punch"));
		m_cbxEditCompetitorOnPunch->setToolTip(tr("Edit or insert competitor on card insert into station."));
		main_tb->addWidget(m_cbxEditCompetitorOnPunch);
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

void CompetitorsWidget::lazyInit()
{
}

void CompetitorsWidget::reset()
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

void CompetitorsWidget::reload()
{
	bool is_relays = eventPlugin()->eventConfig()->isRelays();
	qfs::QueryBuilder qb;
	qb.select2("competitors", "*")
			.select2("classes", "name")
			.select("COALESCE(lastName, '') || ' ' || COALESCE(firstName, '') AS competitorName")
			.from("competitors")
			.orderBy("competitors.id");//.limit(10);
	int class_id = m_cbxClasses->currentData().toInt();
	if(class_id > 0) {
		qb.where("classes.id=" + QString::number(class_id));
	}
	QString join_kind = (class_id > 0)? qfs::QueryBuilder::INNER_JOIN: qfs::QueryBuilder::LEFT_JOIN;
	if(is_relays) {
		qb.join("competitors.id", "runs.competitorId", join_kind);
		qb.join("runs.relayId", "relays.id", join_kind);
		qb.join("relays.classId", "classes.id", join_kind);
	}
	else {
		qb.join("competitors.classId", "classes.id", join_kind);
	}
	m_competitorsModel->setQueryBuilder(qb, false);
	m_competitorsModel->reload();
}

void CompetitorsWidget::editCompetitor_helper(const QVariant &id, int mode, int siid)
{
	qfLogFuncFrame() << "id:" << id << "mode:" << mode;
	m_cbxEditCompetitorOnPunch->setEnabled(false);
	auto *w = new CompetitorWidget();
	w->setWindowTitle(tr("Edit Competitor"));
	qfd::Dialog dlg(QDialogButtonBox::Save | QDialogButtonBox::Cancel, this);
	dlg.setDefaultButton(QDialogButtonBox::Save);
	QPushButton *bt_save_and_next = dlg.buttonBox()->addButton(tr("Save and &next"), QDialogButtonBox::AcceptRole);
	bool save_and_next = false;
	connect(dlg.buttonBox(), &qf::qmlwidgets::DialogButtonBox::clicked, [&save_and_next, bt_save_and_next](QAbstractButton *button) {
		save_and_next = (button == bt_save_and_next);
	});
	dlg.setCentralWidget(w);
	w->load(id, mode);
	auto *doc = qobject_cast<Competitors::CompetitorDocument*>(w->dataController()->document());
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
	connect(doc, &Competitors::CompetitorDocument::saved, ui->tblCompetitors, &qf::qmlwidgets::TableView::rowExternallySaved, Qt::QueuedConnection);
	connect(doc, &Competitors::CompetitorDocument::saved, competitorsPlugin(), &Competitors::CompetitorsPlugin::competitorEdited, Qt::QueuedConnection);
	bool ok = dlg.exec();
	m_cbxEditCompetitorOnPunch->setEnabled(true);
	if(ok && save_and_next) {
		QTimer::singleShot(0, [this]() {
			this->editCompetitor(QVariant(), qf::core::model::DataDocument::ModeInsert);
		});
	}
}

void CompetitorsWidget::editCompetitors(int mode)
{
	if(mode == qfm::DataDocument::ModeDelete) {
		QList<int> sel_rows = ui->tblCompetitors->selectedRowsIndexes();
		if(sel_rows.count() <= 1)
			return;
		if(qfd::MessageBox::askYesNo(this, tr("Realy delete all the selected competitors? This action cannot be reverted."), false)) {
			qfs::Transaction transaction;
			int n = 0;
			for(int ix : sel_rows) {
				int id = ui->tblCompetitors->tableRow(ix).value(ui->tblCompetitors->idColumnName()).toInt();
				if(id > 0) {
					Competitors::CompetitorDocument doc;
					doc.load(id, qfm::DataDocument::ModeDelete);
					doc.drop();
					n++;
				}
			}
			if(n > 0) {
				if(qfd::MessageBox::askYesNo(this, tr("Confirm deletion of %1 competitors.").arg(n), false)) {
					transaction.commit();
					ui->tblCompetitors->reload();
				}
				else {
					transaction.rollback();
				}
			}
		}
	}
}

void CompetitorsWidget::onDbEventNotify(const QString &domain, int connection_id, const QVariant &data)
{
	Q_UNUSED(connection_id)
	qfLogFuncFrame() << "domain:" << domain << "payload:" << data;
	if(m_cbxEditCompetitorOnPunch->isEnabled() && m_cbxEditCompetitorOnPunch->isChecked() && domain == QLatin1String(Event::EventPlugin::DBEVENT_PUNCH_RECEIVED)) {
		quickevent::core::si::PunchRecord punch(data.toMap());
		int siid = punch.siid();
		if(siid > 0 && punch.marking() == quickevent::core::si::PunchRecord::MARKING_ENTRIES) {
			editCompetitorOnPunch(siid);
		}
	}
}

void CompetitorsWidget::editCompetitorOnPunch(int siid)
{
	qfs::Query q;
	q.exec("SELECT id FROM competitors WHERE siId=" + QString::number(siid), qfc::Exception::Throw);
	if(q.next()) {
		int competitor_id = q.value(0).toInt();
		if(competitor_id > 0) {
			editCompetitor_helper(competitor_id, qfm::DataDocument::ModeEdit, 0);
		}
	}
	else {
		editCompetitor_helper(QVariant(), qfm::DataDocument::ModeInsert, siid);
	}
}

void CompetitorsWidget::onCustomContextMenuRequest(const QPoint &pos)
{
	qfLogFuncFrame();
	QAction a_change_class(tr("Set class in selected rows"), nullptr);
	QList<QAction*> lst;
	lst << &a_change_class;
	QAction *a = QMenu::exec(lst, ui->tblCompetitors->viewport()->mapToGlobal(pos));
	if(a == &a_change_class) {
		qfw::dialogs::GetItemInputDialog dlg(this);
		QComboBox *box = dlg.comboBox();
		qfs::Query q;
		q.exec("SELECT id, name FROM classes ORDER BY name");
		while (q.next()) {
			box->addItem(q.value(1).toString(), q.value(0));
		}
		dlg.setWindowTitle(tr("Dialog"));
		dlg.setLabelText(tr("Select class"));
		dlg.setCurrentItemIndex(-1);
		if(dlg.exec()) {
			int class_id = dlg.currentData().toInt();
			if(class_id > 0) {
				qfs::Transaction transaction;
				try {
					QList<int> rows = ui->tblCompetitors->selectedRowsIndexes();
					for(int i : rows) {
						qf::core::utils::TableRow row = ui->tblCompetitors->tableRowRef(i);
						int competitor_id = row.value("competitors.id").toInt();
						q.exec(QString("UPDATE competitors SET classId=%1 WHERE id=%2").arg(class_id).arg(competitor_id), qfc::Exception::Throw);
					}
					transaction.commit();
				}
				catch (std::exception &e) {
					qfError() << e.what();
				}
			}
			ui->tblCompetitors->reload(true);
		}
	}
}
