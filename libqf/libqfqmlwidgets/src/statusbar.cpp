#include "statusbar.h"

#include <qf/core/log.h>

using namespace qf::qmlwidgets;

StatusBar::StatusBar(QWidget *parent) :
	Super(parent)
{
}

qf::qmlwidgets::StatusBar::~StatusBar()
{
	qfLogFuncFrame() << this;
}

void StatusBar::showProgress(const QString &msg, int completed, int total)
{
	Q_UNUSED(completed);
	Q_UNUSED(total);
	showMessage(msg);
}

void StatusBar::showMessage(const QString &message, int timeout)
{
	Super::showMessage(message, timeout);
}

void StatusBar::addPermanentWidget(QWidget *widget, int stretch)
{
	widget->setParent(0);
	Super::addPermanentWidget(widget, stretch);
}

void StatusBar::addWidget(QWidget *widget, int stretch)
{
	widget->setParent(0);
	Super::addWidget(widget, stretch);
}
