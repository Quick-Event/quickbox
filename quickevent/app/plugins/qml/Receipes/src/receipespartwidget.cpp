#include "receipespartwidget.h"
#include "receipeswidget.h"

#include <qf/qmlwidgets/frame.h>

#include <QMetaObject>

ReceipesPartWidget::ReceipesPartWidget(const QString &feature_id, QWidget *parent)
	: Super(feature_id, parent)
{
	setTitle(tr("Receipes"));

	ReceipesWidget *w = new ReceipesWidget();
	centralFrame()->addWidget(w);
	w->settleDownInPartWidget(this);

	QMetaObject::invokeMethod(this, "lazyInit", Qt::QueuedConnection);
}

ReceipesPartWidget::~ReceipesPartWidget()
{
}

void ReceipesPartWidget::lazyInit()
{
}

