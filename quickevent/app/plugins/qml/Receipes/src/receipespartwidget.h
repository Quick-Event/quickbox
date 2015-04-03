#ifndef RECEIPESPARTWIDGET_H
#define RECEIPESPARTWIDGET_H

#include <quickevent/partwidget.h>

class ReceipesPartWidget : public quickevent::PartWidget
{
	Q_OBJECT
private:
	typedef quickevent::PartWidget Super;
public:
	ReceipesPartWidget(const QString &feature_id, QWidget *parent = 0);
	~ReceipesPartWidget() Q_DECL_OVERRIDE;
private:
	Q_SLOT void lazyInit();
};

#endif // RECEIPESPARTWIDGET_H
