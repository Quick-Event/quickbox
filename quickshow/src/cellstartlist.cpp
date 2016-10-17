#include "cellstartlist.h"
#include "ui_cellstartlist.h"

CellStartList::CellStartList(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::CellStartList)
{
	ui->setupUi(this);
}

CellStartList::~CellStartList()
{
	delete ui;
}
