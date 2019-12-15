#include "thispartwidget.h"
#include "competitorswidget.h"

#include <qf/core/log.h>

#include <qf/qmlwidgets/frame.h>

namespace Competitors {

ThisPartWidget::ThisPartWidget(QWidget *parent)
	: Super(parent)
{
	setObjectName("Competitors::PartWidget");
	setFeatureId("Competitors");
	setIconSource(":/quickevent/" + featureId() + "/images/feature");
	setPersistentSettingsId(featureId());
	setTitle(tr("&Competitors"));

	CompetitorsWidget *w = new CompetitorsWidget();
	centralFrame()->addWidget(w);
	w->settleDownInPartWidget(this);

	//QMetaObject::invokeMethod(this, "lazyInit", Qt::QueuedConnection);
}

}
