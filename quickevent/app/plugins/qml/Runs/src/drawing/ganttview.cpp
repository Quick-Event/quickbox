#include "ganttview.h"

#include <qf/core/log.h>

#include <QMouseEvent>

using namespace drawing;

GanttView::GanttView(QWidget *parent)
	: Super(parent)
{

}

void GanttView::wheelEvent(QWheelEvent *ev)
{
	if(ev->orientation() == Qt::Vertical) {
		if(ev->modifiers() == Qt::ControlModifier) {
			int delta = ev->angleDelta().y();
			zoom(delta, ev->pos());
			ev->accept();
			return;
		}
	}
	Super::wheelEvent(ev);
}

void GanttView::mousePressEvent(QMouseEvent* ev)
{
	if (ev->button() == Qt::LeftButton && ev->modifiers() == Qt::ControlModifier) {
		m_dragMouseStartPos = ev->pos();
		setCursor(QCursor(Qt::ClosedHandCursor));
		ev->accept();
		return;
	}
	Super::mousePressEvent(ev);
}

void GanttView::mouseReleaseEvent(QMouseEvent* ev)
{
	setCursor(QCursor(Qt::ArrowCursor));
	Super::mouseReleaseEvent(ev);
}

void GanttView::mouseMoveEvent(QMouseEvent* ev)
{
	if(ev->buttons() == Qt::LeftButton && ev->modifiers() == Qt::ControlModifier) {
		QPoint pos = ev->pos();
		QRect view_rect = QRect(viewport()->pos(), viewport()->size());
		QPoint view_center = view_rect.center();
		QPoint d(pos.x() - m_dragMouseStartPos.x(), pos.y() - m_dragMouseStartPos.y());
		view_center -= d;
		QPointF new_scene_center = mapToScene(view_center);
		centerOn(new_scene_center);
		m_dragMouseStartPos = pos;
		ev->accept();
		return;
	}
	Super::mouseMoveEvent(ev);
}

void GanttView::zoom(int delta, const QPoint &mouse_pos)
{
	qfLogFuncFrame() << "delta:" << delta << "center_pos:" << mouse_pos.x() << mouse_pos.y();
	double factor = delta / 100.;
	if(delta < 0)
		factor = -1 / factor;
	scale(factor, factor);

	QRect view_rect = QRect(viewport()->pos(), viewport()->size());
	QPoint view_center = view_rect.center();
 	QSize view_d(view_center.x() - mouse_pos.x(), view_center.y() - mouse_pos.y());
	view_d /= factor;
	view_center = QPoint(mouse_pos.x() + view_d.width(), mouse_pos.y() + view_d.height());
	QPointF new_scene_center = mapToScene(view_center);
	centerOn(new_scene_center);
}
