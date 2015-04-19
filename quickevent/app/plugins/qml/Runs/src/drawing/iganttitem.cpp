#include "iganttitem.h"
#include "ganttscene.h"

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
