#include "dockwidget.h"
#include "../frame.h"

#include <qf/core/log.h>
#include <qf/core/utils.h>

#include <QShowEvent>
#include <QVBoxLayout>

using namespace qf::qmlwidgets::framework;

DockWidget::DockWidget(QWidget *parent, Qt::WindowFlags flags) :
	Super(parent, flags)
  , framework::IPersistentSettings(this)
{
	qfLogFuncFrame();
}

DockWidget::~DockWidget()
{
	qfLogFuncFrame();
}

bool DockWidget::event(QEvent *ev)
{
	if(ev->type() == QEvent::MouseButtonDblClick) {
		//maximizeOnScreen();
		if(isFloating()) {
			setWindowState(windowState() ^ Qt::WindowFullScreen);
			return true;
		}
	}
	return Super::event(ev);
}
/*
void DockWidget::showEvent(QShowEvent *ev)
{
	Super::showEvent(ev);
	if(!ev->spontaneous()) {
		// There are two kinds of show events: show events caused by the window system (spontaneous), and internal show events.
		// Spontaneous (QEvent::spontaneous()) show events are sent just after the window system shows the window;
		// they are also sent when a top-level window is redisplayed after being iconified. Internal show events are delivered just before the widget becomes visible.
		if(ev->type() == QEvent::Show) {
			emit visibleChanged(true);
		}
		else if(ev->type() == QEvent::Hide) {
			emit visibleChanged(false);
		}
	}
}
*/
void DockWidget::setQmlWidget(QWidget *w)
{
	w->setParent(0);
	w->show();
	Super::setWidget(w);
}

