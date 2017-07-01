#pragma once

#include <qf/qmlwidgets/tableview.h>

class PunchesTableView : public qf::qmlwidgets::TableView
{
	Q_OBJECT
private:
	using Super = qf::qmlwidgets::TableView;
public:
	PunchesTableView(QWidget *parent);

	Q_SIGNAL void codeClassActivated(int class_id, int code);
protected:
	bool edit(const QModelIndex& index, EditTrigger trigger, QEvent* event) Q_DECL_OVERRIDE;
	/*
	void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
	void dragEnterEvent(QDragEnterEvent *event) Q_DECL_OVERRIDE;
	void dragMoveEvent(QDragMoveEvent *event) Q_DECL_OVERRIDE;
	*/
};
