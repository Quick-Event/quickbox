#ifndef DRAWING_GANTTVIEW_H
#define DRAWING_GANTTVIEW_H

#include <QGraphicsView>

namespace drawing {

class GanttView : public QGraphicsView
{
	Q_OBJECT
private:
	typedef QGraphicsView Super;
public:
	GanttView(QWidget *parent = 0);
protected:
	void wheelEvent(QWheelEvent *ev) Q_DECL_OVERRIDE;
	void mousePressEvent(QMouseEvent *ev) Q_DECL_OVERRIDE;
	void mouseReleaseEvent(QMouseEvent *ev) Q_DECL_OVERRIDE;
	void mouseMoveEvent(QMouseEvent *ev) Q_DECL_OVERRIDE;

	void zoom(int delta, const QPoint &mouse_pos);
private:
	QPoint m_dragMouseStartPos;
};

}

#endif
