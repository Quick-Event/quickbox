#pragma once

#include <qf/qmlwidgets/tableview.h>

class PunchesTableView : public qf::qmlwidgets::TableView
{
	Q_OBJECT
private:
	using Super = qf::qmlwidgets::TableView;
public:
	PunchesTableView(QWidget *parent);
protected:
	/*
	void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
	void dragEnterEvent(QDragEnterEvent *event) Q_DECL_OVERRIDE;
	void dragMoveEvent(QDragMoveEvent *event) Q_DECL_OVERRIDE;
	*/
};
