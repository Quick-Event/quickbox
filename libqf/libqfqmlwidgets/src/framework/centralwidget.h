#ifndef QF_QMLWIDGETS_FRAMEWORK_CENTRALWIDGET_H
#define QF_QMLWIDGETS_FRAMEWORK_CENTRALWIDGET_H

#include <QWidget>

class QStackedWidget;

namespace qf {
namespace qmlwidgets {
namespace framework {

class PartWidget;
class MainWindow;

class CentralWidget : public QWidget
{
	Q_OBJECT
private:
	typedef QWidget Super;
public:
	explicit CentralWidget(MainWindow *parent = 0);
public:
	virtual bool setPartActive(int part_index, bool set_active) = 0;

	Q_SIGNAL void partActivated(const QString &feature_id, bool is_active);
public:
	virtual void addPartWidget(PartWidget *widget) = 0;
protected:
	MainWindow *m_frameWork;
};

}}}

#endif // QF_QMLWIDGETS_FRAMEWORK_CENTRALWIDGET_H
