#ifndef RUNSTABLEDIALOGWIDGET_H
#define RUNSTABLEDIALOGWIDGET_H

#include <QWidget>

#include <qf/qmlwidgets/framework/dialogwidget.h>

namespace Ui {
class RunsTableDialogWidget;
}

class RunsTableWidget;

class RunsTableDialogWidget : public qf::qmlwidgets::framework::DialogWidget
{
	Q_OBJECT
private:
	typedef qf::qmlwidgets::framework::DialogWidget Super;
public:
	explicit RunsTableDialogWidget(QWidget *parent = 0);
	~RunsTableDialogWidget();

	RunsTableWidget* runsTableWidget();
	void reload(int stage_id, int class_id, const QString &sort_column = QString(), int select_competitor_id = 0);
private:
	Ui::RunsTableDialogWidget *ui;
};

#endif // RUNSTABLEDIALOGWIDGET_H
