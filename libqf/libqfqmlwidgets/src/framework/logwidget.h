#ifndef QF_QMLWIGGETS_FRAMEWORK_LOGWIDGET_H
#define QF_QMLWIGGETS_FRAMEWORK_LOGWIDGET_H

#include <QWidget>

namespace qf {
namespace qmlwiggets {
namespace framework {

namespace Ui {
class LogWidget;
}

class LogWidget : public QWidget
{
	Q_OBJECT

public:
	explicit LogWidget(QWidget *parent = 0);
	~LogWidget();

private:
	Ui::LogWidget *ui;
};


} // namespace framework
} // namespace qmlwiggets
} // namespace qf
#endif // QF_QMLWIGGETS_FRAMEWORK_LOGWIDGET_H
