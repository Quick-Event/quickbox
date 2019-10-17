#ifndef QF_QMLWIDGETS_MENUBAR_H
#define QF_QMLWIDGETS_MENUBAR_H

#include "qmlwidgetsglobal.h"

#include <QMenuBar>

namespace qf {
namespace qmlwidgets {

class Action;

class QFQMLWIDGETS_DECL_EXPORT MenuBar : public QMenuBar
{
	Q_OBJECT
private:
	typedef QMenuBar Super;
public:
	explicit MenuBar(QWidget *parent = nullptr);
public:
	Q_INVOKABLE qf::qmlwidgets::Action* actionForPath(const QString &path, bool create_if_not_exists = true);
};

}}

#endif // QF_QMLWIDGETS_MENUBAR_H
