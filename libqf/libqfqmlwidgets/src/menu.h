#ifndef QF_QMLWIDGETS_MENU_H
#define QF_QMLWIDGETS_MENU_H

not used for now

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
public:
};

}}

#endif // MENU_H
