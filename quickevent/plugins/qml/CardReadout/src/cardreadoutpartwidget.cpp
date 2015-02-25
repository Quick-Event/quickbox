#include "cardreadoutpartwidget.h"
#include "cardreadoutwidget.h"

#include <qf/qmlwidgets/frame.h>

CardReadoutPartWidget::CardReadoutPartWidget(QWidget *parent)
	: Super(parent)
{
	setTitle(tr("Card readout"));
	CardReadoutWidget *w = new CardReadoutWidget();
	centralFrame()->addWidget(w);
	w->settleDownInPartWidget(this);
}

CardReadoutPartWidget::~CardReadoutPartWidget()
{
}

