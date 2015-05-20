#ifndef RECIPESPARTWIDGET_H
#define RECIPESPARTWIDGET_H

#include <quickevent/partwidget.h>

class RecipesPartWidget : public quickevent::PartWidget
{
	Q_OBJECT
private:
	typedef quickevent::PartWidget Super;
public:
	RecipesPartWidget(const QString &feature_id, QWidget *parent = 0);
	~RecipesPartWidget() Q_DECL_OVERRIDE;
private:
	Q_SLOT void lazyInit();
};

#endif // RECIPESPARTWIDGET_H
