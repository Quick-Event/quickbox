#include "relaysthispartwidget.h"
#include "relayswidget.h"

#include <qf/core/log.h>

#include <qf/qmlwidgets/frame.h>

RelaysThisPartWidget::RelaysThisPartWidget(QWidget *parent)
	: Super(parent)
{
	setPersistentSettingsId("Relays");
	setTitle(tr("&Relays"));

	RelaysWidget *w = new RelaysWidget();
	centralFrame()->addWidget(w);
	w->settleDownInPartWidget(this);

	//QMetaObject::invokeMethod(this, "lazyInit", Qt::QueuedConnection);
}

