#ifndef RUNSTABLEVIEW_H
#define RUNSTABLEVIEW_H

#include <qf/qmlwidgets/tableview.h>

class RunsTableView : public qf::qmlwidgets::TableView
{
	Q_OBJECT
private:
	typedef qf::qmlwidgets::TableView Super;
public:
	RunsTableView(QWidget *parent = nullptr);
	~RunsTableView() Q_DECL_OVERRIDE;
protected:
	/*
	void dragEnterEvent(QDragEnterEvent *event) Q_DECL_OVERRIDE;
	void dragMoveEvent(QDragMoveEvent *event) Q_DECL_OVERRIDE;
	void dropEvent(QDropEvent *event) Q_DECL_OVERRIDE;
	void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
	void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
	*/
};

#endif // RUNSTABLEVIEW_H
