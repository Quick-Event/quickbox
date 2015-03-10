#include "classeswidget.h"
#include "ui_classeswidget.h"

ClassesWidget::ClassesWidget(QWidget *parent) :
	Super(parent),
	ui(new Ui::ClassesWidget)
{
	ui->setupUi(this);
}

ClassesWidget::~ClassesWidget()
{
	delete ui;
}
