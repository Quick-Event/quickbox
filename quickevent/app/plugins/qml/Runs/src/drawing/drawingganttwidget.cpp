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
	setTitle(tr("Drawing tool"));
	setPersistentSettingsId("DrawingToolWidget");
	ui->setupUi(this);

	ui->actSave->setIcon(qf::qmlwidgets::Style::instance()->icon("save"));

	m_ganttScene = new GanttScene(this);
	ui->ganttView->setScene(m_ganttScene);
	m_ganttScene->load();
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
	auto *a_file = menu->actionForPath("drawing");
	a_file->setText(tr("&Drawing"));
	a_file->addActionInto(ui->actSave);
}

void drawing::DrawingGanttWidget::on_actSave_triggered()
{
	m_ganttScene->save();
}
