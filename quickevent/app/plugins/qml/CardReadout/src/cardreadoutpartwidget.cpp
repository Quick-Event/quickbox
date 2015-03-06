#include "cardreadoutpartwidget.h"
#include "cardreadoutwidget.h"

#include <qf/qmlwidgets/frame.h>

#include <QMetaObject>

CardReadoutPartWidget::CardReadoutPartWidget(QWidget *parent)
	: Super(parent)
{
	setTitle(tr("Card readout"));

	CardReadoutWidget *w = new CardReadoutWidget();
	centralFrame()->addWidget(w);
	w->settleDownInPartWidget(this);

	QMetaObject::invokeMethod(this, "lazyInit", Qt::QueuedConnection);
}

CardReadoutPartWidget::~CardReadoutPartWidget()
{
}

void CardReadoutPartWidget::lazyInit()
{
}

