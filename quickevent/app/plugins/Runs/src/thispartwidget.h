#ifndef THISPARTWIDGET_H
#define THISPARTWIDGET_H

#include <quickevent/gui/partwidget.h>

class RunsWidget;

class ThisPartWidget : public quickevent::gui::PartWidget
{
	Q_OBJECT
private:
	typedef quickevent::gui::PartWidget Super;
public:
	ThisPartWidget(QWidget *parent = nullptr);

	RunsWidget* runsWidget() {return m_runsWidget;}

	Q_SIGNAL void selectedStageIdChanged(int stage_id);
private:
	RunsWidget* m_runsWidget = nullptr;
};

#endif // THISPARTWIDGET_H
