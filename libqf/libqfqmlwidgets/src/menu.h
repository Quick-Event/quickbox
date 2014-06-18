#ifndef QF_QMLWIDGETS_MENU_H
#define QF_QMLWIDGETS_MENU_H

#include "qmlwidgetsglobal.h"

#include <QMenu>

namespace qf {
namespace qmlwidgets {

class QFQMLWIDGETS_DECL_EXPORT Menu : public QMenu
{
	Q_OBJECT
private:
	typedef QMenu Super;
public:
	explicit Menu(QWidget *parent = 0);
public slots:
	void addAction(QObject *action);
};

}}

#endif // MENU_H
