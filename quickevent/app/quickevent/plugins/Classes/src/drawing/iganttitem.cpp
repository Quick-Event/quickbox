#include "iganttitem.h"
#include "ganttscene.h"
#include "ganttitem.h"

#include <qf/core/assert.h>

#include <QGraphicsItem>

using namespace drawing;

IGanttItem::IGanttItem(QGraphicsItem *grit)
	: m_graphicsItem(grit)
{
	QF_ASSERT_EX(m_graphicsItem != nullptr, "Bad parent!");
}

GanttScene* IGanttItem::ganttScene()
{
	auto *gs = qobject_cast<GanttScene*>(m_graphicsItem->scene());
	QF_ASSERT_EX(gs != nullptr, "Bad scene!");
	return gs;
}

const GanttScene *IGanttItem::ganttScene() const
{
	auto *gs = qobject_cast<const GanttScene*>(m_graphicsItem->scene());
	QF_ASSERT_EX(gs != nullptr, "Bad scene!");
	return gs;
}

int IGanttItem::pxToMin(int px) const
{
	return ganttScene()->pxToMin(px);
}

int IGanttItem::minToPx(int min) const
{
	return ganttScene()->minToPx(min);
}

const GanttItem *IGanttItem::ganttItem() const
{
	const GanttItem *ret = nullptr;
	for (const QGraphicsItem *it = this->m_graphicsItem; it; it = it->parentItem()) {
		ret = dynamic_cast<const GanttItem*>(it);
		if(ret)
			break;
	}
	QF_ASSERT_EX(ret != nullptr, "Bad parent!");
	return ret;
}

GanttItem *IGanttItem::ganttItem()
{
	return const_cast<GanttItem*>(((const IGanttItem*)this)->ganttItem());
}

