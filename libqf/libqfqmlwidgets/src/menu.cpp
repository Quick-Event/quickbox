#include "menu.h"

#include <qf/core/log.h>

using namespace qf::qmlwidgets;

Menu::Menu(QWidget *parent) :
	Super(parent)
{
}

void Menu::addAction(QObject *action)
{
	QAction *a = qobject_cast<QAction*>(action);
	if(!a) {
		qfWarning() << action << "is not kind of QAction";
	}
	else {
		Super::addAction(a);
	}
}
