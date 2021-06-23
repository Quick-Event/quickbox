#include "speakerwidget.h"
#include "speakerthispartwidget.h"

#include <qf/core/log.h>

#include <qf/qmlwidgets/frame.h>

SpeakerThisPartWidget::SpeakerThisPartWidget(QWidget *parent)
	: Super(parent)
{
	setPersistentSettingsId("Speaker");
	setTitle(tr("&Speaker"));

	SpeakerWidget *w = new SpeakerWidget();
	centralFrame()->addWidget(w);
	w->settleDownInPartWidget(this);

	//QMetaObject::invokeMethod(this, "lazyInit", Qt::QueuedConnection);
}

