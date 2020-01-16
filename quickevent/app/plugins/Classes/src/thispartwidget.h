#pragma once

#include <quickevent/gui/partwidget.h>

namespace Classes {

class ThisPartWidget : public quickevent::gui::PartWidget
{
	Q_OBJECT
private:
	typedef quickevent::gui::PartWidget Super;
public:
	ThisPartWidget(QWidget *parent = nullptr);
};

}
