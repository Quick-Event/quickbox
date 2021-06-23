#ifndef RUNSTHISPARTWIDGET_H
#define RUNSTHISPARTWIDGET_H

#include <quickevent/gui/partwidget.h>

class RunsThisPartWidget : public quickevent::gui::PartWidget
{
	Q_OBJECT
private:
	typedef quickevent::gui::PartWidget Super;
public:
	RunsThisPartWidget(QWidget *parent = nullptr);

	//RunsWidget* runsWidget() {return m_runsWidget;}

	Q_SIGNAL void selectedStageIdChanged(int stage_id);
private:
	//RunsWidget* m_runsWidget = nullptr;
};


#endif // RUNSTHISPARTWIDGET_H
