#ifndef RELAYSTHISPARTWIDGET_H
#define RELAYSTHISPARTWIDGET_H

#include <quickevent/gui/partwidget.h>

class RelaysThisPartWidget : public quickevent::gui::PartWidget
{
	Q_OBJECT
private:
	typedef quickevent::gui::PartWidget Super;
public:
	RelaysThisPartWidget(QWidget *parent = nullptr);
protected:
	//void reload() Q_DECL_OVERRIDE;
};

#endif // RELAYSTHISPARTWIDGET_H
