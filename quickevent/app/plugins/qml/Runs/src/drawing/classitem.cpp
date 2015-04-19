#include "classitem.h"
#include "ganttscene.h"
#include "startslotitem.h"

#include <qf/core/sql/query.h>

#include <QPainter>

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
	: Super(parent), IGanttItem(this)
{
	int du_px = ganttScene()->displayUnit();
	m_classText = new QGraphicsTextItem(this);
	m_courseText = new QGraphicsTextItem(this);
	m_courseText->setPos(0, 2 * du_px);
	m_classdefsText = new QGraphicsTextItem(this);
	m_classdefsText->setPos(0, 4 * du_px);
	QRect r;
	r.setHeight(6 * du_px + du_px/2);
	setRect(r);
}

const ClassData &ClassItem::data() const
{
	return m_data;
}

void ClassItem::setData(const ClassData &data)
{
	m_data = data;
}

void ClassItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	QColor c_runner = color();
	QColor c_free = c_runner;
	c_free.setAlpha(128);
	QColor c_first;
	c_first.setHsv((c_runner.hue() + 64) % 256, 255, 255, 128);
	auto r = rect();
	QRectF r1 = r;
	r1.setWidth(minToPx(1));
	painter->fillRect(r, c_free);
	auto dt = data();
	int interval = dt.startIntervalMin();
	for (int i = 0; i < runsAndVacantCount(); ++i) {
		r1.moveLeft(minToPx(i * interval));
		QColor c = (i == 0)? c_first: c_runner;
		painter->fillRect(r1, c);
	}
	Super::paint(painter, option, widget);
}

int ClassItem::runsAndVacantCount() const
{
	auto dt = data();
	int cnt = dt.runsCount();
	int vacants = (dt.vacantEvery() > 0)? cnt / dt.vacantEvery(): 0;
	cnt = dt.vacantsBefore() + cnt + vacants + dt.vacantsAfter();
	return cnt;
}

int ClassItem::durationMin() const
{
	auto dt = data();
	return runsAndVacantCount() * dt.startIntervalMin();
}

QColor ClassItem::color() const
{
	auto dt = data();
	int hue = dt.firstCode() % 100;
	QColor c;
	c.setHsvF(hue / 100., 1, 1, 0.5);
	return c;
}

void ClassItem::updateGeometry()
{
	auto dt = data();
	qfLogFuncFrame() << dt.className() << "runners:" << dt.runsCount();
	QRectF r = rect();
	int cnt = dt.runsCount();
	int vacants = (dt.vacantEvery() > 0)? cnt / dt.vacantEvery(): 0;
	cnt = dt.vacantsBefore() + cnt + vacants + dt.vacantsAfter();
	r.setWidth(minToPx(durationMin()));
	setRect(r);
	//setBrush(color());
	m_classText->setHtml(QString("<b>%1</b> %2+%3").arg(dt.className()).arg(dt.runsCount()).arg(runsAndVacantCount() - dt.runsCount()));
	m_courseText->setHtml(QString("<b>%1</b> (%2)").arg(dt.firstCode()).arg(dt.courseId()));
	dt.setStartTimeMin(pxToMin(pos().x()));
	m_classdefsText->setPlainText(QString("%1 / %2").arg(dt.startTimeMin()).arg(dt.startIntervalMin()));
	{
		QString tool_tip;
		// HTML tooltip can cause word wrap
		tool_tip = "<html><body>";
		tool_tip += QString("class: <b>%1</b>, %2 runners + %3 vacants<br/>").arg(dt.className()).arg(dt.runsCount()).arg(runsAndVacantCount() - dt.runsCount());
		tool_tip += QString("first code <b>%1</b>, course name: %2<br/>").arg(dt.firstCode()).arg(dt.courseName());
		tool_tip += QString("vacants before: %1, every: %2, after: %3<br/>").arg(dt.vacantsBefore()).arg(dt.vacantEvery()).arg(dt.vacantsAfter());
		tool_tip += QString("start: %1, duration: %2, end: %3").arg(dt.startTimeMin()).arg(durationMin()).arg(dt.startTimeMin() + durationMin());
		tool_tip += "</body></html>";
		tool_tip.replace(' ', "&nbsp;");
		setToolTip(tool_tip);
	}
}



