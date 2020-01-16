#include "thispartwidget.h"
#include "runswidget.h"

#include <qf/core/log.h>

#include <qf/qmlwidgets/frame.h>

ThisPartWidget::ThisPartWidget(QWidget *parent)
	: Super(parent)
{
	setPersistentSettingsId("Runs");
	setTitle(tr("&Runs"));

	auto *w = new RunsWidget();
	centralFrame()->addWidget(w);
	w->settleDownInPartWidget(this);

	connect(w, &RunsWidget::selectedStageIdChanged, this, &ThisPartWidget::selectedStageIdChanged);
}

