#pragma once

#include <quickevent/gui/partwidget.h>

class CompetitorsThisPartWidget : public quickevent::gui::PartWidget
{
	Q_OBJECT
private:
	typedef quickevent::gui::PartWidget Super;
public:
	CompetitorsThisPartWidget(QWidget *parent = nullptr);
protected:
	//void reload() Q_DECL_OVERRIDE;
};

