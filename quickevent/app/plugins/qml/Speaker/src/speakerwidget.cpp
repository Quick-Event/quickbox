#include "speakerwidget.h"
#include "ui_speakerwidget.h"

#include "thispartwidget.h"

#include "Speaker/speakerplugin.h"

#include "Event/eventplugin.h"

#include <quickevent/og/siid.h>

#include <qf/qmlwidgets/dialogs/dialog.h>
#include <qf/qmlwidgets/framework/mainwindow.h>
#include <qf/qmlwidgets/framework/plugin.h>
#include <qf/qmlwidgets/toolbar.h>
#include <qf/qmlwidgets/combobox.h>

#include <qf/core/model/sqltablemodel.h>
#include <qf/core/sql/querybuilder.h>
#include <qf/core/assert.h>

#include <QLabel>

namespace qfs = qf::core::sql;
namespace qfw = qf::qmlwidgets;
namespace qff = qf::qmlwidgets::framework;
namespace qfd = qf::qmlwidgets::dialogs;
namespace qfm = qf::core::model;

static Event::EventPlugin* eventPlugin()
{
	qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
	auto *plugin = qobject_cast<Event::EventPlugin*>(fwk->plugin("Event"));
	QF_ASSERT_EX(plugin != nullptr, "Bad Event plugin!");
	return plugin;
}

SpeakerWidget::SpeakerWidget(QWidget *parent) :
	Super(parent),
	ui(new Ui::SpeakerWidget)
{
	ui->setupUi(this);

	ui->tblPunchesToolBar->setTableView(ui->tblPunches);

	ui->tblPunches->setCloneRowEnabled(false);
	ui->tblPunches->setPersistentSettingsId("tblPunches");
	ui->tblPunches->setRowEditorMode(qfw::TableView::EditRowsMixed);
	ui->tblPunches->setInlineEditSaveStrategy(qfw::TableView::OnEditedValueCommit);
	qfm::SqlTableModel *m = new qfm::SqlTableModel(this);
	m->addColumn("id").setReadOnly(true);
	m->addColumn("code", tr("Code"));
	m->addColumn("siId", tr("SI")).setReadOnly(true).setCastType(qMetaTypeId<quickevent::og::SiId>());
	m->addColumn("punchTime", tr("Punch time"));
	ui->tblPunches->setTableModel(m);
	m_punchesModel = m;

	//connect(ui->tblPunches, SIGNAL(editRowInExternalEditor(QVariant,int)), this, SLOT(editCompetitor(QVariant,int)), Qt::QueuedConnection);

	QMetaObject::invokeMethod(this, "lazyInit", Qt::QueuedConnection);
}

SpeakerWidget::~SpeakerWidget()
{
	delete ui;
}

void SpeakerWidget::settleDownInPartWidget(ThisPartWidget *part_widget)
{
	connect(part_widget, SIGNAL(resetPartRequest()), this, SLOT(reset()));
	connect(part_widget, SIGNAL(reloadPartRequest()), this, SLOT(reload()));
	/*
	qfw::Action *a = part_widget->menuBar()->actionForPath("station", true);
	a->setText("&Station");
	a->addActionInto(m_actCommOpen);
	*/
	//qfw::ToolBar *main_tb = part_widget->toolBar("main", true);
	//main_tb->addAction(m_actCommOpen);
	/*
	QLabel *lbl_class;
	{
		lbl_class = new QLabel(tr("&Class "));
		main_tb->addWidget(lbl_class);
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
	lbl_class->setBuddy(m_cbxClasses);
	*/
}

void SpeakerWidget::lazyInit()
{
}

void SpeakerWidget::reset()
{
	if(!eventPlugin()->isEventOpen()) {
		m_punchesModel->clearRows();
		return;
	}
	reload();
	/*
	{
		m_cbxClasses->blockSignals(true);
		m_cbxClasses->loadItems(true);
		m_cbxClasses->insertItem(0, tr("--- all ---"), 0);
		connect(m_cbxClasses, SIGNAL(currentDataChanged(QVariant)), this, SLOT(reload()), Qt::UniqueConnection);
		m_cbxClasses->blockSignals(false);
	}
	*/
}

void SpeakerWidget::reload()
{
	qfs::QueryBuilder qb;
	qb.select2("punches", "*")
			//.select2("classes", "name")
			//.select("COALESCE(lastName, '') || ' ' || COALESCE(firstName, '') AS competitorName")
			.from("punches")
			//.join("competitors.classId", "classes.id")
			.orderBy("id");//.limit(10);
	//int class_id = m_cbxClasses->currentData().toInt();
	//if(class_id > 0) {
	//	qb.where("competitors.classId=" + QString::number(class_id));
	//}
	m_punchesModel->setQueryBuilder(qb);
	m_punchesModel->reload();
}
