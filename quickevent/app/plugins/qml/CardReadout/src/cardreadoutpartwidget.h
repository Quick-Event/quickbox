#ifndef CARDREADOUTPARTWIDGET_H
#define CARDREADOUTPARTWIDGET_H

#include <quickevent/partwidget.h>

class CardReadoutPartWidget : public quickevent::PartWidget
{
	Q_OBJECT
private:
	typedef quickevent::PartWidget Super;
public:
	CardReadoutPartWidget(QWidget *parent = 0);
	~CardReadoutPartWidget() Q_DECL_OVERRIDE;
private:
	Q_SLOT void lazyInit();
};

#endif // CARDREADOUTPARTWIDGET_H
