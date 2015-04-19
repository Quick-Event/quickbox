#include "startslotitem.h"
#include "classitem.h"
#include "ganttscene.h"

#include <QtGlobal>

using namespace drawing;

StartSlotItem::StartSlotItem(QGraphicsItem *parent)
	: Super(parent), IGanttItem(this)
{
	m_textSlotNo = new QGraphicsTextItem(this);
	//setFlag(QGraphicsItem::ItemIsSelectable);
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

static constexpr int LABEL_WIDTH_DU = 5;

void StartSlotItem::updateGeometry()
{
	QRectF r;
	m_textSlotNo->setPlainText(QString::number(slotNumber()));
	int label_width = minToPx(ganttScene()->duToMin(LABEL_WIDTH_DU));
	m_textSlotNo->setPos(-label_width, 0);
	int pos_x = 0;
	double h = m_textSlotNo->boundingRect().height();
	for (int i = 0; i < classItemCount(); ++i) {
		ClassItem *it = classItem(i);
		it->setPos(pos_x, 0);
		it->updateGeometry();
		pos_x += it->rect().width();
		h = qMax(h, it->rect().height());
	}
	r.setHeight(h);
	r.setWidth(label_width + pos_x);
	r.moveLeft(-label_width);
	//qfInfo() << r.left() << r.top() << r.width() << r.height();
	setRect(r);
}
