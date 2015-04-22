#ifndef DRAWING_STARTSLOTHEADER_H
#define DRAWING_STARTSLOTHEADER_H

#include "iganttitem.h"

#include <QGraphicsRectItem>
#include <QCoreApplication>

namespace drawing {

class StartSlotItem;

class StartSlotHeader : public QGraphicsRectItem, public IGanttItem
{
	Q_DECLARE_TR_FUNCTIONS(drawing::StartSlotHeader)
private:
	typedef QGraphicsRectItem Super;
public:
	StartSlotHeader(QGraphicsItem * parent = 0);

	int minHeight();
	void updateGeometry();
protected:
	void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) Q_DECL_OVERRIDE;
private:
	StartSlotItem* startSlotItem();
private:
	QGraphicsTextItem *m_textSlotNo;
};

} // namespace drawing

#endif // DRAWING_STARTSLOTHEADER_H
