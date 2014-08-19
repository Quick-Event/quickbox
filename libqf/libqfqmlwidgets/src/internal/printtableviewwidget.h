#ifndef QF_QMLWIDGETS_INTERNAL_PRINTTABLEVIEWWIDGET_H
#define QF_QMLWIDGETS_INTERNAL_PRINTTABLEVIEWWIDGET_H

#include "../framework/dialogwidget.h"

namespace qf {
namespace qmlwidgets {
namespace internal {

namespace Ui {
class PrintTableViewWidget;
}

class PrintTableViewWidget : public qf::qmlwidgets::framework::DialogWidget
{
	Q_OBJECT
private:
	typedef qf::qmlwidgets::framework::DialogWidget Super;
public:
	explicit PrintTableViewWidget(QWidget *parent = 0);
	~PrintTableViewWidget() Q_DECL_OVERRIDE;
private:
	Ui::PrintTableViewWidget *ui;
};

}}}

#endif // QF_QMLWIDGETS_INTERNAL_PRINTTABLEVIEWWIDGET_H
