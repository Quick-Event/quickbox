#include "competitorswidget.h"
#include "ui_competitorswidget.h"

CompetitorsWidget::CompetitorsWidget(QWidget *parent) :
	Super(parent),
	ui(new Ui::CompetitorsWidget)
{
	ui->setupUi(this);
}

CompetitorsWidget::~CompetitorsWidget()
{
	delete ui;
}
