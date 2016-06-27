#include "menubar.h"
//#include "menu.h"
#include "action.h"

#include <qf/core/log.h>
#include <qf/core/string.h>
#include <qf/core/assert.h>

#include <QStringList>
#include <QMenu>

using namespace qf::qmlwidgets;

MenuBar::MenuBar(QWidget *parent) :
	Super(parent)
{
}

Action* MenuBar::actionForPath(const QString &path, bool create_if_not_exists)
{
	qfLogFuncFrame() << path;
	QWidget *parent_w = this;
	Action *ret = nullptr;
	QStringList path_list = qf::core::String(path).splitAndTrim('/');
	Q_FOREACH(auto id, path_list) {
		qfDebug() << id << "of path:" << path;
		if(!parent_w) {
			/// recent action was not a menu one
			qfWarning() << "Attempt to traverse through not menu action before:" << id << "in:" << path_list.join("/");
			break;
		}
		ret = nullptr;
		Q_FOREACH(auto a, parent_w->actions()) {
			qfDebug() << a;
			Action *act = qobject_cast<Action*>(a);
			//if(!ret) continue;
			QF_ASSERT(act!=nullptr, "bad action", return ret);
			if(act->oid() == id) {
				qfDebug() << "\t found action" << a;
				ret = act;
				break;
			}
		}
		if(!ret) {
			if(!create_if_not_exists) {
				break;
			}
			QMenu *m = new QMenu(parent_w);
			ret = new Action(parent_w);
			ret->setMenu(m);
			ret->setOid(id);
			ret->setText(id);
			qfDebug() << "\t created menu" << ret;
			parent_w->addAction(ret);
		}
		parent_w = ret->menu();
	}
	if(create_if_not_exists) {
		QF_CHECK(ret != nullptr, "internal error, returned action is NULL");
	}
	qfDebug() << "\t RETURN:" << ret;
	return ret;
}

