#ifndef QF_QMLWIDGETS_FRAMEWORK_DIALOGWIDGET_H
#define QF_QMLWIDGETS_FRAMEWORK_DIALOGWIDGET_H

#include "../qmlwidgetsglobal.h"
//#include "ipersistentsettings.h"

#include <QWidget>

namespace qf {
namespace qmlwidgets {
namespace framework {

class QFQMLWIDGETS_DECL_EXPORT DialogWidget : public QWidget//, public IPersistentSettings
{
	Q_OBJECT
private:
	typedef QWidget Super;
public:
	explicit DialogWidget(QWidget *parent = 0);
	~DialogWidget() Q_DECL_OVERRIDE;
};

}}}

#endif // QF_QMLWIDGETS_FRAMEWORK_DIALOGWIDGET_H
