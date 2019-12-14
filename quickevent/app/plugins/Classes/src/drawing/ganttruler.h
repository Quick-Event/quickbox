#ifndef DRAWING_GANTTRULER_H
#define DRAWING_GANTTRULER_H

#include "iganttitem.h"

#include <QGraphicsRectItem>

namespace drawing {

class GanttRuler : public QGraphicsRectItem, public IGanttItem
{
private:
	typedef QGraphicsRectItem Super;
public:
	GanttRuler(QGraphicsItem * parent = 0);

	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0) Q_DECL_OVERRIDE;
};

}

#endif // DRAWING_GANTTRULER_H
