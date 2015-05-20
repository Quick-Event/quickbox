#include "recipespartwidget.h"
#include "recipeswidget.h"

#include <qf/qmlwidgets/frame.h>

#include <QMetaObject>

RecipesPartWidget::RecipesPartWidget(const QString &feature_id, QWidget *parent)
	: Super(feature_id, parent)
{
	setTitle(tr("Receipes"));

	RecipesWidget *w = new RecipesWidget();
	centralFrame()->addWidget(w);
	w->settleDownInPartWidget(this);

	QMetaObject::invokeMethod(this, "lazyInit", Qt::QueuedConnection);
}

RecipesPartWidget::~RecipesPartWidget()
{
}

void RecipesPartWidget::lazyInit()
{
}

