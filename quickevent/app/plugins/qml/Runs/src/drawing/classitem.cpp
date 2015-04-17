#include "classitem.h"
#include "ganttscene.h"

#include <qf/core/sql/query.h>

using namespace drawing;

ClassData::ClassData(const qf::core::sql::Query &q)
{
	for(auto s : {"id", "className", "classId", "courseId", "startSlotIndex", "startSlotPosition", "startTimeMin", "startIntervalMin", "vacantsBefore", "vacantsBefore", "vacantEvery", "vacantsAfter", "firstCode", "runsCount"}) {
		insert(s, q.value(s));
	}
}

ClassItem::ClassItem(QGraphicsItem *parent)
	: Super(parent)
{

}

const ClassData &ClassItem::data() const
{
	return m_data;
}

void ClassItem::setData(const ClassData &data)
{
	m_data = data;
}

GanttScene *ClassItem::ganttScene()
{
	auto *ret = qobject_cast<GanttScene*>(scene());
	QF_ASSERT_EX(ret != nullptr, "Bad scene!");
	return ret;
}

void ClassItem::updateGeometry()
{
	auto dt = data();
	qfLogFuncFrame() << dt.className() << "runners:" << dt.runsCount();
	QRect r;
	int du = ganttScene()->displayUnit();
	r.setHeight(3 * du);
	int cnt = dt.runsCount();
	int vacants = (dt.vacantEvery() > 0)? cnt / dt.vacantEvery(): 0;
	cnt = dt.vacantsBefore() + cnt + vacants + dt.vacantsAfter();
	r.setWidth(cnt * dt.startIntervalMin() * du / 3);
	setRect(r);
	{
		int hue = dt.firstCode() % 100;
		QColor c;
		c.setHsvF(hue / 100., 1, 1);
		setBrush(c);
	}
}



