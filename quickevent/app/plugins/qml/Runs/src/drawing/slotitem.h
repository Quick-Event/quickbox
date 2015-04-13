#ifndef DRAWING_SLOTITEM_H
#define DRAWING_SLOTITEM_H

#include <QGraphicsRectItem>

namespace drawing {

class SlotItem : public QGraphicsRectItem
{
private:
	typedef QGraphicsRectItem Super;
public:
	SlotItem(QGraphicsItem * parent = 0);
};

}

#endif // DRAWING_SLOTITEM_H
