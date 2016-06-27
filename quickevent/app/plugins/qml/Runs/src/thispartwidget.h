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

	Q_SIGNAL void selectedStageIdChanged(int stage_id);
};

#endif // THISPARTWIDGET_H
