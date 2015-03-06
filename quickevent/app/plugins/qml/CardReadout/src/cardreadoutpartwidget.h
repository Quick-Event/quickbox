#ifndef CARDREADOUTPARTWIDGET_H
#define CARDREADOUTPARTWIDGET_H

#include <qf/qmlwidgets/framework/partwidget.h>

class CardReadoutPartWidget : public qf::qmlwidgets::framework::PartWidget
{
	Q_OBJECT
private:
	typedef qf::qmlwidgets::framework::PartWidget Super;
public:
	CardReadoutPartWidget(QWidget *parent = 0);
	~CardReadoutPartWidget() Q_DECL_OVERRIDE;
private:
	Q_SLOT void lazyInit();
};

#endif // CARDREADOUTPARTWIDGET_H
