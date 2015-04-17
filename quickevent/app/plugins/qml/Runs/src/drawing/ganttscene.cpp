#include "ganttitem.h"
#include "ganttscene.h"

#include <QFontMetrics>

using namespace drawing;

GanttScene::GanttScene(QObject * parent)
	: Super(parent)
{
	setDisplayUnit(QFontMetrics(QFont()).lineSpacing() / 2);

}

void GanttScene::load()
{
	clear();
	auto *it = new GanttItem();
	addItem(it);
	it->load();
}


