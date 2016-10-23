#include "competitorswidget.h"
#include "ui_competitorswidget.h"
#include "competitorwidget.h"
#include "thispartwidget.h"

#include "Competitors/competitordocument.h"
#include "Competitors/competitorsplugin.h"

#include "Event/eventplugin.h"

#include <quickevent/si/siid.h>
#include <quickevent/si/punchrecord.h>

#include <qf/qmlwidgets/dialogs/dialog.h>
#include <qf/qmlwidgets/dialogs/messagebox.h>
#include <qf/qmlwidgets/framework/mainwindow.h>
#include <qf/qmlwidgets/framework/plugin.h>
#include <qf/qmlwidgets/toolbar.h>
#include <qf/qmlwidgets/combobox.h>

#include <qf/core/model/sqltablemodel.h>
#include <qf/core/sql/querybuilder.h>
#include <qf/core/sql/transaction.h>
#include <qf/core/assert.h>

#include <QCheckBox>
#include <QLabel>

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
	m->addColumn("competitorName", tr("Name"));
	m->addColumn("registration", tr("Reg")).setReadOnly(true);
	m->addColumn("siId", tr("SI")).setReadOnly(true).setCastType(qMetaTypeId<quickevent::si::SiId>());
	m->addColumn("ranking", tr("Ranking"));
	m->addColumn("note", tr("Note"));
	ui->tblCompetitors->setTableModel(m);
	m_competitorsModel = m;

	//connect(ui->tblCompetitors, SIGNAL(editRowInExternalEditor(QVariant,int)), this, SLOT(editCompetitor(QVariant,int)), Qt::QueuedConnection);
	connect(ui->tblCompetitors, &qfw::TableView::editRowInExternalEditor, this, &CompetitorsWidget::editCompetitor, Qt::QueuedConnection);
	connect(ui->tblCompetitors, &qfw::TableView::editSelectedRowsInExternalEditor, this, &CompetitorsWidget::editCompetitors, Qt::QueuedConnection);

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

void CompetitorsWidget::editCompetitor_helper(const QVariant &id, int mode, int siid)
{
	qfLogFuncFrame() << "id:" << id << "mode:" << mode;
	m_cbxEditCompetitorOnPunch->setEnabled(false);
	auto *w = new CompetitorWidget();
	w->setWindowTitle(tr("Edit Competitor"));
	qfd::Dialog dlg(QDialogButtonBox::Save | QDialogButtonBox::Cancel, this);
	dlg.setDefaultButton(QDialogButtonBox::Save);
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
	dlg.exec();
	m_cbxEditCompetitorOnPunch->setEnabled(true);
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
		quickevent::si::PunchRecord punch(data.toMap());
		int siid = punch.siid();
		if(siid > 0 && punch.marking() == quickevent::si::PunchRecord::MARKING_ENTRIES) {
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
