#include "menubar.h"
#include "menu.h"

#include <qf/core/log.h>
#include <qf/core/string.h>

#include <QStringList>

using namespace qf::qmlwidgets;

MenuBar::MenuBar(QWidget *parent) :
	QMenuBar(parent)
{
}

Menu* MenuBar::ensureMenuOnPath(const QString &path)
{
	qfLogFuncFrame() << path;
	QWidget *parent_w = this;
	QStringList path_list = qf::core::String(path).splitAndTrim('/');
	for(auto id : path_list) {
		qfDebug() << id << parent_w;
		QAction *act = nullptr;
		for(auto a : parent_w->actions()) {
			if(a->objectName() == id) {
				qfDebug() << "\t found action" << a;
				act = a;
				break;
			}
		}
		if(!act) {
			Menu *m = new Menu();
			act = m->menuAction();
			act->setObjectName(id);
			act->setText("Untitled");
			qfDebug() << "\t created menu" << act;
			parent_w->addAction(act);
		}
		parent_w = act->menu();
	}
	Menu *ret = qobject_cast<Menu*>(parent_w);
	Q_ASSERT(ret != nullptr);
	return ret;
}

