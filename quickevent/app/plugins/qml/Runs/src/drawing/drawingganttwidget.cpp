#include "drawingganttwidget.h"
#include "ui_drawingganttwidget.h"

#include "ganttscene.h"

using namespace drawing;

DrawingGanttWidget::DrawingGanttWidget(QWidget *parent) :
	Super(parent),
	ui(new Ui::DrawingGanttWidget)
{
	setTitle(tr("Drawing tool"));
	setPersistentSettingsId("DrawingToolWidget");
	ui->setupUi(this);

	m_ganttScene = new GanttScene(this);
	ui->ganttView->setScene(m_ganttScene);
	m_ganttScene->load();
}

DrawingGanttWidget::~DrawingGanttWidget()
{
	delete ui;
}
