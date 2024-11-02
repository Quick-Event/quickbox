#include "dlgtableviewcopyspecial.h"
#include "ui_dlgtableviewcopyspecial.h"

using namespace qf::qmlwidgets::internal;

DlgTableViewCopySpecial::DlgTableViewCopySpecial(QWidget *parent)
	: Super(parent)
{
	ui = new Ui::DlgTableViewCopySpecial;
	ui->setupUi(this);

	ui->lstReplaceEscapes->addItem(tr("Quote field if needed"), static_cast<int>(TableView::ReplaceEscapes::QuoteIfNeeded));
	ui->lstReplaceEscapes->addItem(tr("Never"), static_cast<int>(TableView::ReplaceEscapes::Never));
	ui->lstReplaceEscapes->addItem(tr("Always"), static_cast<int>(TableView::ReplaceEscapes::Always));
	ui->lstReplaceEscapes->setCurrentIndex(0);
}

DlgTableViewCopySpecial::~DlgTableViewCopySpecial()
{
	delete ui;
}

QString DlgTableViewCopySpecial::fieldsQuotes()
{
	return ui->edFieldsQuotes->text();
}

QString DlgTableViewCopySpecial::fieldsSeparator()
{
	return ui->edFieldsSeparator->text();
}

QString DlgTableViewCopySpecial::rowsSeparator()
{
	return ui->edRowsSeparator->text();
}

qf::qmlwidgets::TableView::ReplaceEscapes DlgTableViewCopySpecial::replaceEscapes()
{
	return static_cast<TableView::ReplaceEscapes>(ui->lstReplaceEscapes->currentData().toInt());
}

