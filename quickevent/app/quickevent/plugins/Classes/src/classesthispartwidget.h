#pragma once

#include <quickevent/gui/partwidget.h>

class ClassesThisPartWidget : public quickevent::gui::PartWidget
{
	Q_OBJECT
private:
	typedef quickevent::gui::PartWidget Super;
public:
	ClassesThisPartWidget(QWidget *parent = nullptr);
};

