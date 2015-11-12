#ifndef RUNSTABLEWIDGET_H
#define RUNSTABLEWIDGET_H

#include <QWidget>

class RunsTableModel;
class RunsTableItemDelegate;

namespace Ui {
class RunsTableWidget;
}

class RunsTableWidget : public QWidget
{
	Q_OBJECT
private:
	typedef QWidget Super;
public:
	explicit RunsTableWidget(QWidget *parent = nullptr);
	~RunsTableWidget() Q_DECL_OVERRIDE;

	void clear();
	void reload(int stage_id, int class_id = 0, const QString &sort_column = QString(), int select_competitor_id = 0);

	RunsTableModel* runsModel() {return m_runsModel;}
private:
	void onCustomContextMenuRequest(const QPoint &pos);
private:
	Ui::RunsTableWidget *ui;
	RunsTableModel *m_runsModel;
	RunsTableItemDelegate *m_runsTableItemDelegate;
};

#endif // RUNSTABLEWIDGET_H
