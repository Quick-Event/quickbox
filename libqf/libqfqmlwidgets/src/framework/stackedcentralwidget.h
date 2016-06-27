#ifndef STACKEDCENTRALWIDGET_H
#define STACKEDCENTRALWIDGET_H

#include "../qmlwidgetsglobal.h"
#include "centralwidget.h"

class QStackedWidget;

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
	PartWidget* partWidget(int part_index);

	bool setActivePart(int part_index, bool set_active)  Q_DECL_OVERRIDE;
	int featureToIndex(const QString &feature_id) Q_DECL_OVERRIDE;
private:
	QStackedWidget *m_centralWidget;
	PartSwitch *m_partSwitch;
};

}}}

#endif // STACKEDCENTRALWIDGET_H
