#ifndef DRAWING_DRAWINGTOOLWIDGET_H
#define DRAWING_DRAWINGTOOLWIDGET_H

#include <qf/qmlwidgets/framework/dialogwidget.h>

namespace drawing {

namespace Ui {
class DrawingToolWidget;
}

class DrawingToolWidget : public qf::qmlwidgets::framework::DialogWidget
{
	Q_OBJECT
private:
	typedef qf::qmlwidgets::framework::DialogWidget Super;
public:
	explicit DrawingToolWidget(QWidget *parent = 0);
	~DrawingToolWidget();

private:
	Ui::DrawingToolWidget *ui;
};

}

#endif // DRAWING_DRAWINGTOOLWIDGET_H
