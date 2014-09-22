
//
// Author: Frantisek Vacek <fanda.vacek@volny.cz>, (C) 2011
//
// Copyright: See COPYING file that comes with this distribution
//

#include "ui_itemvalueeditorwidget.h"
#include "itemvalueeditorwidget.h"

#include <qf/core/log.h>

using namespace qf::qmlwidgets::reports;

//=================================================
//             ItemValueEditorWidget
//=================================================
ItemValueEditorWidget::ItemValueEditorWidget(QWidget *parent)
	: Super(parent)
{
	ui = new Ui::ItemValueEditorWidget;
	ui->setupUi(this);

	//--setXmlConfigPersistentId("ReportViewWidget/ItemValueEditorWidget", true);
}

ItemValueEditorWidget::~ItemValueEditorWidget()
{
	delete ui;
}

QVariant ItemValueEditorWidget::value() const
{
	return ui->edText->toPlainText();
}

void ItemValueEditorWidget::setValue(const QVariant& val)
{
	ui->edText->setPlainText(val.toString());
}
