#include "thispartwidget.h"
#include "runswidget.h"

#include <qf/core/log.h>

#include <qf/qmlwidgets/frame.h>

ThisPartWidget::ThisPartWidget(QWidget *parent)
	: Super(parent)
{
	setTitle(tr("Runs"));

	RunsWidget *w = new RunsWidget();
	centralFrame()->addWidget(w);
	w->settleDownInPartWidget(this);

	//QMetaObject::invokeMethod(this, "lazyInit", Qt::QueuedConnection);
}

