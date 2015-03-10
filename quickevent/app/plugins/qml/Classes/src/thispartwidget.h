#ifndef THISPARTWIDGET_H
#define THISPARTWIDGET_H

#include <qf/qmlwidgets/framework/partwidget.h>

class ThisPartWidget : public qf::qmlwidgets::framework::PartWidget
{
	Q_OBJECT
private:
	typedef qf::qmlwidgets::framework::PartWidget Super;
public:
	ThisPartWidget(QWidget *parent = nullptr);
};

#endif // THISPARTWIDGET_H
