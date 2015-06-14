#include "receiptspartwidget.h"
#include "receiptswidget.h"

#include <qf/qmlwidgets/frame.h>

#include <QMetaObject>

ReceiptsPartWidget::ReceiptsPartWidget(const QString &feature_id, QWidget *parent)
	: Super(feature_id, parent)
{
	setTitle(tr("Receipts"));

	ReceiptsWidget *w = new ReceiptsWidget();
	centralFrame()->addWidget(w);
	w->settleDownInPartWidget(this);

	QMetaObject::invokeMethod(this, "lazyInit", Qt::QueuedConnection);
}

ReceiptsPartWidget::~ReceiptsPartWidget()
{
}

void ReceiptsPartWidget::lazyInit()
{
}

