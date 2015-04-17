#include "startslotitem.h"
#include "classitem.h"
#include "ganttscene.h"

#include <QtGlobal>

using namespace drawing;

StartSlotItem::StartSlotItem(QGraphicsItem *parent)
	: Super(parent)
{
	m_slotNoText = new QGraphicsTextItem(this);
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

GanttScene *StartSlotItem::ganttScene()
{
	auto *ret = qobject_cast<GanttScene*>(scene());
	QF_ASSERT_EX(ret != nullptr, "Bad scene!");
	return ret;
}

void StartSlotItem::updateGeometry()
{
	QRectF r;
	int du = ganttScene()->displayUnit();
	m_slotNoText->setPlainText(QString::number(slotNumber()));
	int pos_x = 5 * du;
	double h = m_slotNoText->boundingRect().height();
	for (int i = 0; i < classItemCount(); ++i) {
		ClassItem *it = classItem(i);
		it->setPos(pos_x, 0);
		it->updateGeometry();
		pos_x += it->rect().width();
		h = qMax(h, it->rect().height());
	}
	r.setHeight(h);
	r.setWidth(pos_x);
	setRect(r);
}


