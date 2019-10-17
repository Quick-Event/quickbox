#ifndef DRAWING_DRAWINGGANTTWIDGET_H
#define DRAWING_DRAWINGGANTTWIDGET_H

#include <qf/qmlwidgets/framework/dialogwidget.h>

class QLineEdit;

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
	explicit DrawingGanttWidget(QWidget *parent = nullptr);
	~DrawingGanttWidget() override;

	void settleDownInDialog(qf::qmlwidgets::dialogs::Dialog *dlg) Q_DECL_OVERRIDE;

	void load(int stage_id);
private slots:
	void on_actSave_triggered();
	void on_actFind_triggered();
private:
	Ui::DrawingGanttWidget *ui;
	QLineEdit *m_edFind = nullptr;
	GanttScene *m_ganttScene;
};

}

#endif
