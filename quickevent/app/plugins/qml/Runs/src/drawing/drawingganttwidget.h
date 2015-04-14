#ifndef DRAWING_DRAWINGGANTTWIDGET_H
#define DRAWING_DRAWINGGANTTWIDGET_H

#include <qf/qmlwidgets/framework/dialogwidget.h>

namespace drawing {

namespace Ui {
class DrawingGanttWidget;
}

class DrawingGanttWidget : public qf::qmlwidgets::framework::DialogWidget
{
	Q_OBJECT
private:
	typedef qf::qmlwidgets::framework::DialogWidget Super;
public:
	explicit DrawingGanttWidget(QWidget *parent = 0);
	~DrawingGanttWidget();

private:
	Ui::DrawingGanttWidget *ui;
};

}

#endif
