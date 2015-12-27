#include "editcodeswidget.h"
#include "ui_editcodeswidget.h"

EditCodesWidget::EditCodesWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::EditCodesWidget)
{
	ui->setupUi(this);
}

EditCodesWidget::~EditCodesWidget()
{
	delete ui;
}
