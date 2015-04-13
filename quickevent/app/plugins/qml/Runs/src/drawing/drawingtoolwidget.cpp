#include "drawingtoolwidget.h"
#include "ui_drawingtoolwidget.h"

using namespace drawing;

DrawingToolWidget::DrawingToolWidget(QWidget *parent) :
	Super(parent),
	ui(new Ui::DrawingToolWidget)
{
	setTitle(tr("Drawing tool"));
	setPersistentSettingsId("DrawingToolWidget");
	ui->setupUi(this);
}

DrawingToolWidget::~DrawingToolWidget()
{
	delete ui;
}
