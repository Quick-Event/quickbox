#ifndef EVENT_STAGEWIDGET_H
#define EVENT_STAGEWIDGET_H

#include <qf/qmlwidgets/framework/datadialogwidget.h>

#include <QWidget>

namespace Event {

namespace Ui {
class StageWidget;
}

class StageWidget : public qf::qmlwidgets::framework::DataDialogWidget
{
	Q_OBJECT
private:
	typedef qf::qmlwidgets::framework::DataDialogWidget Super;
public:
	explicit StageWidget(QWidget *parent = 0);
	~StageWidget();

private:
	Ui::StageWidget *ui;
};

}

#endif
