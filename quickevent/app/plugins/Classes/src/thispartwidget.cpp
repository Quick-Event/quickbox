#include "thispartwidget.h"
#include "classeswidget.h"

#include <qf/qmlwidgets/frame.h>

namespace Classes {

ThisPartWidget::ThisPartWidget(QWidget *parent)
	: Super(parent)
{
	setObjectName("Classes::PartWidget");
	setFeatureId("Classes");
	setIconSource(":/quickevent/" + featureId() + "/images/feature");
	setPersistentSettingsId(featureId());
	setTitle(tr("Classes"));

	ClassesWidget *w = new ClassesWidget();
	centralFrame()->addWidget(w);
	w->settleDownInPartWidget(this);

	//QMetaObject::invokeMethod(this, "lazyInit", Qt::QueuedConnection);
}

}
