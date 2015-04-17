#include "classitem.h"
#include "ganttscene.h"
#include "startslotitem.h"

#include <qf/core/sql/query.h>

using namespace drawing;

ClassData::ClassData(const qf::core::sql::Query &q)
{
	for(auto s : {"id",
		"className", "classId",
		"courseName", "courseId",
		"startSlotIndex", "startSlotPosition",
		"startTimeMin", "startIntervalMin",
		"vacantsBefore", "vacantsBefore", "vacantEvery", "vacantsAfter",
		"firstCode",
		"runsCount"}) {
		insert(s, q.value(s));
	}
}

ClassItem::ClassItem(QGraphicsItem *parent)
	: Super(parent)
{
	m_classText = new QGraphicsTextItem(this);
	m_courseText = new QGraphicsTextItem(this);
	m_courseText->setPos(0, 2 * du());
	m_classdefsText = new QGraphicsTextItem(this);
	m_classdefsText->setPos(0, 4 * du());
}

const ClassData &ClassItem::data() const
{
	return m_data;
}

void ClassItem::setData(const ClassData &data)
{
	m_data = data;
}

int ClassItem::du() const
{
	auto *gs = qobject_cast<const GanttScene*>(scene());
	QF_ASSERT_EX(gs != nullptr, "Bad scene!");
	return gs->displayUnit();
}

int ClassItem::duration() const
{
	auto dt = data();
	int cnt = dt.runsCount();
	int vacants = (dt.vacantEvery() > 0)? cnt / dt.vacantEvery(): 0;
	cnt = dt.vacantsBefore() + cnt + vacants + dt.vacantsAfter();
	return cnt * dt.startIntervalMin();
}

StartSlotItem *ClassItem::startSlotItem()
{
	auto *ret = dynamic_cast<StartSlotItem*>(parentItem());
	QF_ASSERT_EX(ret != nullptr, "Bad parent!");
	return ret;
}

void ClassItem::updateGeometry()
{
	auto dt = data();
	qfLogFuncFrame() << dt.className() << "runners:" << dt.runsCount();
	QRect r;
	int u = du();
	r.setHeight(6 * u + u/2);
	int cnt = dt.runsCount();
	int vacants = (dt.vacantEvery() > 0)? cnt / dt.vacantEvery(): 0;
	cnt = dt.vacantsBefore() + cnt + vacants + dt.vacantsAfter();
	r.setWidth(startSlotItem()->minToPos(duration()));
	setRect(r);
	{
		int hue = dt.firstCode() % 100;
		QColor c;
		c.setHsvF(hue / 100., 1, 1);
		setBrush(c);
	}
	m_classText->setHtml(QString("<b>%1</b> %2 runners").arg(dt.className()).arg(dt.runsCount()));
	m_courseText->setHtml(QString("<b>%1</b> (%2)").arg(dt.firstCode()).arg(dt.courseId()));
	dt.setStartTimeMin(startSlotItem()->posToMin(pos().x()));
	m_classdefsText->setPlainText(QString("%1 interval %2").arg(dt.startTimeMin()).arg(dt.startIntervalMin()));
}



