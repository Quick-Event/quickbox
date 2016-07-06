#include "ganttitem.h"
#include "ganttscene.h"

#include <QFontMetrics>

using namespace drawing;

GanttScene::GanttScene(QObject * parent)
	: Super(parent), m_ganttItem()
{
	setDisplayUnit(QFontMetrics(QFont()).lineSpacing() / 2);

}

void GanttScene::load(int stage_id)
{
	clear();
	m_ganttItem = new GanttItem();
	addItem(m_ganttItem);
	m_ganttItem->load(stage_id);
}

void GanttScene::save()
{
	if(m_ganttItem)
		m_ganttItem->save();
}

int GanttScene::duToMin(int n) const
{
	return pxToMin(n * displayUnit());
}

int GanttScene::pxToMin(int px) const
{
	static int px_per_min = minToPx(1);
	return px / px_per_min;
}

int GanttScene::minToPx(int min) const
{
	// shoul be always natural number;
	static int px_per_min = 3 * displayUnit() / 2;
	return min * px_per_min;
}

