#include "startslotitem.h"
#include "classitem.h"

using namespace drawing;

StartSlotItem::StartSlotItem(QGraphicsItem *parent)
	: Super(parent)
{

}

ClassItem *StartSlotItem::addClassItem()
{
	return classItem(classItemCount());
}

int StartSlotItem::classItemCount()
{
	return m_classItems.count();
}

ClassItem *StartSlotItem::classItem(int ix)
{
	while(ix >= classItemCount()) {
		m_classItems << new ClassItem(this);
	}
	return m_classItems.value(ix);
}

const StartSlotData& StartSlotItem::data() const
{
	return m_data;
}

void StartSlotItem::setData(const StartSlotData &data)
{
	m_data = data;
}


