#include "drawingganttwidget.h"
#include "ui_drawingganttwidget.h"

using namespace drawing;

DrawingGanttWidget::DrawingGanttWidget(QWidget *parent) :
	Super(parent),
	ui(new Ui::DrawingGanttWidget)
{
	setTitle(tr("Drawing tool"));
	setPersistentSettingsId("DrawingToolWidget");
	ui->setupUi(this);
}

DrawingGanttWidget::~DrawingGanttWidget()
{
	delete ui;
}
