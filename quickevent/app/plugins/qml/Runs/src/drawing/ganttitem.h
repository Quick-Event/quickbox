#ifndef DRAWING_GANTTITEM_H
#define DRAWING_GANTTITEM_H

#include <QGraphicsRectItem>

namespace drawing {

class SlotItem;

class GanttItem : public QGraphicsRectItem
{
private:
	typedef QGraphicsRectItem Super;
public:
	GanttItem(QGraphicsItem * parent = 0);

	void load();
	void save();
private:
	SlotItem* addSlot() {return slot(slotCount());}
	int slotCount() { return m_slots.count(); }
	SlotItem *slot(int ix);
private:
	QList<SlotItem*> m_slots;
};

}

#endif // DRAWING_GANTTITEM_H
