#ifndef THISPARTWIDGET_H
#define THISPARTWIDGET_H

#include <quickevent/gui/partwidget.h>

class ThisPartWidget : public quickevent::gui::PartWidget
{
	Q_OBJECT
private:
	typedef quickevent::gui::PartWidget Super;
public:
	ThisPartWidget(QWidget *parent = nullptr);
};

#endif // THISPARTWIDGET_H
