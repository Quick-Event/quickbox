#include "resultscell.h"
#include "ui_resultscell.h"

ResultsCell::ResultsCell(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::ResultsCell)
{
	ui->setupUi(this);
	setStyleSheet("" "");
}

ResultsCell::~ResultsCell()
{
	delete ui;
}
