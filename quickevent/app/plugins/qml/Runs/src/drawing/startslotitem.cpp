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

int StartSlotItem::du() const
{
	auto *gs = qobject_cast<const GanttScene*>(scene());
	QF_ASSERT_EX(gs != nullptr, "Bad scene!");
	return gs->displayUnit();
}

static constexpr int LABEL_WIDTH_MIN = 5;

int StartSlotItem::posToMin(int pos) const
{
	return pxToMin(pos) - LABEL_WIDTH_MIN;
}

int StartSlotItem::minToPos(int min) const
{
	return minToPx(LABEL_WIDTH_MIN + min);
}

int StartSlotItem::pxToMin(int px) const
{
	static int _du = du();
	return px / _du;
}

int StartSlotItem::minToPx(int min) const
{
	static int _du = du();
	return min * _du;
}

void StartSlotItem::updateGeometry()
{
	QRectF r;
	int u = du();
	m_slotNoText->setPlainText(QString::number(slotNumber()));
	int pos_x = minToPos(0);
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



