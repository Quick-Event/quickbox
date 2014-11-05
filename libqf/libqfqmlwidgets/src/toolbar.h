#ifndef QF_QMLWIDGETS_TOOLBAR_H
#define QF_QMLWIDGETS_TOOLBAR_H

#include "qmlwidgetsglobal.h"

#include <QToolBar>

namespace qf {
namespace qmlwidgets {

//class Action;

class QFQMLWIDGETS_DECL_EXPORT ToolBar : public QToolBar
{
	Q_OBJECT
private:
	typedef QToolBar Super;
public:
	ToolBar(QWidget *parent = nullptr);
	~ToolBar() Q_DECL_OVERRIDE;
};

}}

#endif // QF_QMLWIDGETS_TOOLBAR_H

