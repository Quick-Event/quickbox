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

QObject* MenuBar::itemForPath(const QString &path, bool create_if_not_exists)
{
	qfLogFuncFrame() << path;
	QWidget *parent_w = this;
	QAction *act = nullptr;
	QStringList path_list = qf::core::String(path).splitAndTrim('/');
	for(auto id : path_list) {
		qfDebug() << id << parent_w;
		if(!parent_w) {
			/// recent action was not a menu one
			qfWarning() << "Attempt to traverse thorough not menu action before:" << id << "in:" << path_list.join("/");
			break;
		}
		for(auto a : parent_w->actions()) {
			if(a->objectName() == id) {
				qfDebug() << "\t found action" << a;
				act = a;
				break;
			}
		}
		if(!act) {
			if(!create_if_not_exists) {
				break;
			}
			Menu *m = new Menu();
			act = m->menuAction();
			act->setObjectName(id);
			act->setText("Untitled");
			qfDebug() << "\t created menu" << act;
			parent_w->addAction(act);
		}
		parent_w = act->menu();
	}
	QObject *ret = nullptr;
	if(act) {
		ret = qobject_cast<Menu*>(act->menu());
		if(!ret)
			ret = act;
	}
	if(create_if_not_exists) {
		Q_ASSERT(act != nullptr);
	}
	return ret;
}

