#ifndef QF_QMLWIDGETS_FRAMEWORK_CENTRALWIDGET_H
#define QF_QMLWIDGETS_FRAMEWORK_CENTRALWIDGET_H

#include "../qmlwidgetsglobal.h"

#include <QWidget>

namespace qf {
namespace qmlwidgets {
namespace framework {

class PartWidget;
class MainWindow;

class QFQMLWIDGETS_DECL_EXPORT CentralWidget : public QWidget
{
	Q_OBJECT
private:
	typedef QWidget Super;
public:
	explicit CentralWidget(MainWindow *parent = nullptr);
public:
	Q_SLOT virtual bool setActivePart(int part_index, bool set_active) = 0;
	Q_SLOT bool setActivePart(const QString &feature_id, bool set_active)
	{
		return setActivePart(featureToIndex(feature_id), set_active);
	}
	Q_INVOKABLE virtual int featureToIndex(const QString &feature_id) = 0;

	Q_SIGNAL void partActivated(const QString &feature_id, bool is_active);
	Q_SIGNAL void partActivated(int part_index, bool is_active);
public:
	virtual void addPartWidget(PartWidget *widget) = 0;
};

}}}

#endif // QF_QMLWIDGETS_FRAMEWORK_CENTRALWIDGET_H
