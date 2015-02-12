#include "cardreadoutwidget.h"
#include "ui_cardreadoutwidget.h"

#include <qf/core/log.h>

CardReadoutWidget::CardReadoutWidget(QWidget *parent) :
	Super(parent),
	ui(new Ui::CardReadoutWidget)
{
	ui->setupUi(this);
	//connect(ui->btClearLog, &QToolButton::clicked, ui->txtLog, &QPlainTextEdit::clear);
}

CardReadoutWidget::~CardReadoutWidget()
{
	delete ui;
}

