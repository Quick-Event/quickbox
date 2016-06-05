#include "codeclassresultswidget.h"
#include "ui_codeclassresultswidget.h"

CodeClassResultsWidget::CodeClassResultsWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::CodeClassResultsWidget)
{
	ui->setupUi(this);
}

CodeClassResultsWidget::~CodeClassResultsWidget()
{
	delete ui;
}
