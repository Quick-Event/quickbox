#include "printtableviewwidget.h"
#include "ui_printtableviewwidget.h"

using namespace qf::qmlwidgets::internal;

PrintTableViewWidget::PrintTableViewWidget(QWidget *parent) :
	Super(parent),
	ui(new Ui::PrintTableViewWidget)
{
	ui->setupUi(this);
}

PrintTableViewWidget::~PrintTableViewWidget()
{
	delete ui;
}
