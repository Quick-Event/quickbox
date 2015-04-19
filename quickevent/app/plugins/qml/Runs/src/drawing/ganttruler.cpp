#include "ganttruler.h"
#include "ganttscene.h"

#include <qf/core/log.h>

#include <QPainter>

using namespace drawing;

GanttRuler::GanttRuler(QGraphicsItem *parent)
	: Super(parent), IGanttItem(this)
{

}

void GanttRuler::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	Super::paint(painter, option, widget);
	int duration = pxToMin(rect().width());
	int big_tick_length_min = 10;
	int min_tick_px = minToPx(1);
	int tick_h = ganttScene()->displayUnit() / 3;
	QPen p;
	painter->setPen(p);
	for (int i = 0; i <= duration; ++i) {
		QPoint p1(i * min_tick_px, 0);
		QPoint p2(p1);
		if(i % big_tick_length_min) {
			p2.setY(-tick_h);
		}
		else {
			p2.setY(-tick_h * 3);
			QString s = QString::number(i);
			QRectF r = painter->boundingRect(0, 0, 0, 0, Qt::TextDontClip, s);
			r.moveLeft(p1.x() - r.width() / 2);
			r.moveTop(rect().top());
			//qfInfo() << s << r.left() << r.top() << r.width() << r.height();
			painter->drawText(r, s);
		}
		painter->drawLine(p1, p2);
	}
}


