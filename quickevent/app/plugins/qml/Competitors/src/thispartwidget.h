#ifndef THISPARTWIDGET_H
#define THISPARTWIDGET_H

#include <quickevent/partwidget.h>

class ThisPartWidget : public quickevent::PartWidget
{
	Q_OBJECT
private:
	typedef quickevent::PartWidget Super;
public:
	ThisPartWidget(QWidget *parent = nullptr);
protected:
	//void reload() Q_DECL_OVERRIDE;
};

#endif // THISPARTWIDGET_H
