#ifndef SPEAKERTHISPARTWIDGET_H
#define SPEAKERTHISPARTWIDGET_H

#include <quickevent/gui/partwidget.h>

class SpeakerThisPartWidget : public quickevent::gui::PartWidget
{
	Q_OBJECT
private:
	typedef quickevent::gui::PartWidget Super;
public:
	SpeakerThisPartWidget(QWidget *parent = nullptr);
protected:
	//void reload() Q_DECL_OVERRIDE;
};

#endif // SPEAKERTHISPARTWIDGET_H
