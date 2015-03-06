#include "dockwidget.h"
#include "../frame.h"

#include <qf/core/log.h>
#include <qf/core/utils.h>

#include <QVBoxLayout>

using namespace qf::qmlwidgets::framework;

DockWidget::DockWidget(QWidget *parent, Qt::WindowFlags flags) :
	Super(parent, flags)
{
	qfLogFuncFrame();
}

DockWidget::~DockWidget()
{
	qfLogFuncFrame();
}

void DockWidget::setQmlWidget(QWidget *w)
{
	w->setParent(0);
	w->show();
	Super::setWidget(w);
}

