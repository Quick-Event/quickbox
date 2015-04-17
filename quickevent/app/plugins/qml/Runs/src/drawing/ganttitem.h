#ifndef DRAWING_GANTTITEM_H
#define DRAWING_GANTTITEM_H

#include <qf/core/utils.h>

#include <QGraphicsRectItem>
#include <QVariantList>

namespace Event {
class EventPlugin;
}
namespace drawing {

class StartSlotItem;

class DrawingConfig : public QVariantMap
{
public:
	explicit DrawingConfig(const QVariantMap &m = QVariantMap()) : QVariantMap(m) {}

	QF_VARIANTMAP_FIELD(QVariantList, s, setS, tartSlots)
};


class GanttItem : public QGraphicsRectItem
{
private:
	typedef QGraphicsRectItem Super;
public:
	GanttItem(QGraphicsItem * parent = 0);

	void load();
	void save();

	void updateGeometry();
private:
	StartSlotItem* addStartSlotItem() {return startSlotItem(startSlotItemCount());}
	int startSlotItemCount() { return m_startSlotItems.count(); }
	StartSlotItem *startSlotItem(int ix);
	Event::EventPlugin* eventPlugin();
private:
	QList<StartSlotItem*> m_startSlotItems;
	DrawingConfig m_drawingConfig;
};

}

#endif // DRAWING_GANTTITEM_H
