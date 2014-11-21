#include "dlgtableviewcopyspecial.h"
#include "ui_dlgtableviewcopyspecial.h"

using namespace qf::qmlwidgets::internal;

DlgTableViewCopySpecial::DlgTableViewCopySpecial(QWidget *parent)
	: Super(parent)
{
	ui = new Ui::DlgTableViewCopySpecial;
	ui->setupUi(this);
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

bool DlgTableViewCopySpecial::replaceEscapes()
{
	return ui->chkReplaceEscapes->isChecked();
}

