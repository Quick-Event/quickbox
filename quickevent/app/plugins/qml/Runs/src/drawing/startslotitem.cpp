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

int StartSlotItem::classItemCount()
{
	return m_classItems.count();
}

void StartSlotItem::insertClassItem(int ix, ClassItem *it)
{
	QF_ASSERT(it != nullptr, "Item == NULL", return);
	m_classItems.insert(ix, it);
}

ClassItem *StartSlotItem::classItemAt(int ix)
{
	QF_ASSERT_EX(ix >= 0 && ix < classItemCount(), QString("Invalid item index %1, item count %2!").arg(ix).arg(classItemCount()));
	return m_classItems[ix];
}

ClassItem *StartSlotItem::addClassItem()
{
	auto *it = new ClassItem(this);
	insertClassItem(classItemCount(), it);
	return it;
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
		ClassItem *it = classItemAt(i);
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
