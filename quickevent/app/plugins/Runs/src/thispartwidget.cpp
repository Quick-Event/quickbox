#include "thispartwidget.h"
#include "runswidget.h"

#include <qf/core/log.h>

#include <qf/qmlwidgets/frame.h>

ThisPartWidget::ThisPartWidget(QWidget *parent)
	: Super(parent)
{
	setPersistentSettingsId("Runs");
	setTitle(tr("&Runs"));

	m_runsWidget = new RunsWidget();
	centralFrame()->addWidget(m_runsWidget);
	m_runsWidget->settleDownInPartWidget(this);

	connect(m_runsWidget, &RunsWidget::selectedStageIdChanged, this, &ThisPartWidget::selectedStageIdChanged);
}

