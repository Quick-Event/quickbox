#include "codeclassresultswidget.h"
#include "ui_codeclassresultswidget.h"

#include <QJsonObject>

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

QJsonObject CodeClassResultsWidget::saveSetup()
{
	QJsonObject ret;
	ret["classId"] = 0;
	ret["codeId"] = 0;
	return ret;
}
