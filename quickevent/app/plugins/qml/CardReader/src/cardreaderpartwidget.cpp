#include "cardreaderpartwidget.h"
#include "cardreaderwidget.h"

#include <qf/qmlwidgets/frame.h>

#include <QMetaObject>

CardReaderPartWidget::CardReaderPartWidget(const QString &feature_id, QWidget *parent)
	: Super(feature_id, parent)
{
	setPersistentSettingsId("CardReader");
	setTitle(tr("Card reader"));

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

