#include "drawingganttwidget.h"
#include "ui_drawingganttwidget.h"

#include "ganttscene.h"

#include <qf/qmlwidgets/style.h>
#include <qf/qmlwidgets/toolbar.h>
#include <qf/qmlwidgets/menubar.h>
#include <qf/qmlwidgets/action.h>
#include <qf/qmlwidgets/dialogs/dialog.h>

using namespace drawing;

DrawingGanttWidget::DrawingGanttWidget(QWidget *parent) :
	Super(parent),
	ui(new Ui::DrawingGanttWidget)
{
	setTitle(tr("Draw tool"));
	setPersistentSettingsId("DrawingToolWidget");
	ui->setupUi(this);

	ui->actSave->setIcon(qf::qmlwidgets::Style::instance()->icon("save"));

	m_ganttScene = new GanttScene(this);
	ui->ganttView->setScene(m_ganttScene);
}

DrawingGanttWidget::~DrawingGanttWidget()
{
	delete ui;
}

void DrawingGanttWidget::settleDownInDialog(qf::qmlwidgets::dialogs::Dialog *dlg)
{
	qf::qmlwidgets::ToolBar *tb = dlg->toolBar("main", true);
	tb->addAction(ui->actSave);

	auto *menu = dlg->menuBar();
	auto *a_file = menu->actionForPath("draw");
	a_file->setText(tr("&Draw"));
	a_file->addActionInto(ui->actSave);
}

void DrawingGanttWidget::load(int stage_id)
{
	setTitle(tr("E%1 Draw tool").arg(stage_id));
	m_ganttScene->load(stage_id);
}

void drawing::DrawingGanttWidget::on_actSave_triggered()
{
	m_ganttScene->save();
}
