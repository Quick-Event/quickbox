#include "ganttitem.h"
#include "slotitem.h"

using namespace drawing;

GanttItem::GanttItem(QGraphicsItem *parent)
	: Super(parent)
{

}

SlotItem *GanttItem::slot(int ix)
{
	while(ix <= slotCount()) {
		m_slots << new SlotItem(this);
	}
	return m_slots.value(ix);
}

