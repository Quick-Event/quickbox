#ifndef RECEIPTSPARTWIDGET_H
#define RECEIPTSPARTWIDGET_H

#include <quickevent/gui/partwidget.h>

class ReceiptsPartWidget : public quickevent::gui::PartWidget
{
	Q_OBJECT
private:
	typedef quickevent::gui::PartWidget Super;
public:
	ReceiptsPartWidget(const QString &feature_id, QWidget *parent = 0);
	~ReceiptsPartWidget() Q_DECL_OVERRIDE;
private:
	Q_SLOT void lazyInit();
};

#endif // RECEIPTSPARTWIDGET_H
