#include "cardreaderpartwidget.h"
#include "cardreaderwidget.h"

#include <qf/qmlwidgets/frame.h>

#include <QMetaObject>

CardReaderPartWidget::CardReaderPartWidget(QWidget *parent)
	: Super(parent)
{
	setTitle(tr("Card readout"));

	CardReaderWidget *w = new CardReaderWidget();
	centralFrame()->addWidget(w);
	w->settleDownInPartWidget(this);

	QMetaObject::invokeMethod(this, "lazyInit", Qt::QueuedConnection);
}

CardReaderPartWidget::~CardReaderPartWidget()
{
}

void CardReaderPartWidget::lazyInit()
{
}

