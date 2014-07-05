#ifndef STACKEDCENTRALWIDGET_H
#define STACKEDCENTRALWIDGET_H

#include "../qmlwidgetsglobal.h"
#include "centralwidget.h"

namespace qf {
namespace qmlwidgets {
namespace framework {

class PartWidget;
class PartSwitch;

class QFQMLWIDGETS_DECL_EXPORT StackedCentralWidget : public CentralWidget
{
	Q_OBJECT
private:
	typedef CentralWidget Super;
public:
	explicit StackedCentralWidget(MainWindow *parent = 0);
	~StackedCentralWidget() Q_DECL_OVERRIDE;
public:
	void addPartWidget(PartWidget *widget) Q_DECL_OVERRIDE;

	bool setPartActive(int part_index, bool set_active)  Q_DECL_OVERRIDE;
	PartWidget* partWidget(int part_index);
private:
	QStackedWidget *m_centralWidget;
	PartSwitch *m_partSwitch;
};

}}}

#endif // STACKEDCENTRALWIDGET_H
