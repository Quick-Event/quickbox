#ifndef QF_QMLWIDGETS_ACTIONGROUP_H
#define QF_QMLWIDGETS_ACTIONGROUP_H

#include "qmlwidgetsglobal.h"

#include <QActionGroup>

namespace qf {
namespace qmlwidgets {

class QFQMLWIDGETS_DECL_EXPORT ActionGroup : public QActionGroup
{
	Q_OBJECT
private:
	typedef QActionGroup Super;
public:
	ActionGroup(QObject *parent = nullptr);

	Q_INVOKABLE QAction* addAction(QAction *action) {return Super::addAction(action);}
};

} // namespace qmlwidgets
} // namespace qf

#endif // QF_QMLWIDGETS_ACTIONGROUP_H
