#ifndef DRAWING_DRAWINGGANTTWIDGET_H
#define DRAWING_DRAWINGGANTTWIDGET_H

#include <qf/qmlwidgets/framework/dialogwidget.h>

namespace drawing {

namespace Ui {
class DrawingGanttWidget;
}

class GanttScene;

class DrawingGanttWidget : public qf::qmlwidgets::framework::DialogWidget
{
	Q_OBJECT
private:
	typedef qf::qmlwidgets::framework::DialogWidget Super;
public:
	explicit DrawingGanttWidget(QWidget *parent = 0);
	~DrawingGanttWidget();

	void settleDownInDialog(qf::qmlwidgets::dialogs::Dialog *dlg) Q_DECL_OVERRIDE;
private slots:
	void on_actSave_triggered();
private:
	Ui::DrawingGanttWidget *ui;
	GanttScene *m_ganttScene;
};

}

#endif
