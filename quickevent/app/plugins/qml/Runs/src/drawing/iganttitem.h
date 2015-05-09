#ifndef DRAWING_IGANTTITEM_H
#define DRAWING_IGANTTITEM_H

class QGraphicsItem;

namespace drawing {

class GanttScene;
class GanttItem;

class IGanttItem
{
public:
	IGanttItem(QGraphicsItem *grit);
	virtual ~IGanttItem() {}
public:
	GanttScene* ganttScene();
	const GanttScene* ganttScene() const;

	int pxToMin(int px) const;
	int minToPx(int min) const;

	const GanttItem *ganttItem() const;
	GanttItem *ganttItem();
private:
	QGraphicsItem *m_graphicsItem;
};

} // namespace drawing

#endif // DRAWING_IGANTTITEM_H
