#ifndef CARDREADERPARTWIDGET_H
#define CARDREADERPARTWIDGET_H

#include <quickevent/partwidget.h>

class CardReaderPartWidget : public quickevent::PartWidget
{
	Q_OBJECT
private:
	typedef quickevent::PartWidget Super;
public:
	CardReaderPartWidget(const QString &feature_id, QWidget *parent = 0);
	~CardReaderPartWidget() Q_DECL_OVERRIDE;
private:
	Q_SLOT void lazyInit();
};

#endif // CARDREADERPARTWIDGET_H
