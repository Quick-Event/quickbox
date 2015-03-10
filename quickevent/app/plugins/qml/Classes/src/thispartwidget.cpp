#include "thispartwidget.h"
#include "classeswidget.h"

#include <qf/qmlwidgets/frame.h>

ThisPartWidget::ThisPartWidget(QWidget *parent)
	: Super(parent)
{
	setTitle(tr("Classes"));

	ClassesWidget *w = new ClassesWidget();
	centralFrame()->addWidget(w);
	//w->settleDownInPartWidget(this);

	//QMetaObject::invokeMethod(this, "lazyInit", Qt::QueuedConnection);
}

