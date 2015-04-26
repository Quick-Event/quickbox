#ifndef DRAWING_STARTSLOTITEM_H
#define DRAWING_STARTSLOTITEM_H

#include "iganttitem.h"

#include <qf/core/utils.h>
#include <qf/core/exception.h>

#include <QGraphicsRectItem>

namespace drawing {

class ClassItem;
class GanttItem;
class StartSlotHeader;

class StartSlotData : public QVariantMap
{
public:
	explicit StartSlotData(const QVariantMap &m = QVariantMap()) : QVariantMap(m) {}

	QF_VARIANTMAP_FIELD2(int, s, setS, tartOffset, 0)
	QF_VARIANTMAP_FIELD2(bool, is, set, Locked, false)
	QF_VARIANTMAP_FIELD2(bool, is, set, IgnoreClassClashCheck, false)
};

class StartSlotItem : public QGraphicsRectItem, public IGanttItem
{
private:
	typedef QGraphicsRectItem Super;
public:
	StartSlotItem(QGraphicsItem * parent = 0);

	QF_FIELD_IMPL2(int, s, S, lotNumber, 0)

public:
	ClassItem* addClassItem();
	int classItemCount() const;
	int classItemIndex(const ClassItem *it) const;
	void insertClassItem(int ix, ClassItem *it);
	ClassItem* classItemAt(int ix, bool throw_ex = qf::core::Exception::Throw);
	ClassItem* takeClassItemAt(int ix);

	void setStartOffset(int start_offset);
	int startOffset();

	void setLocked(bool b);
	bool isLocked() const;

	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0) Q_DECL_OVERRIDE;

	void updateGeometry();
	void setClassAreaWidth(int px);

	const StartSlotData& data() const;
	void setData(const StartSlotData &data);

	void dragEnterEvent(QGraphicsSceneDragDropEvent *event) Q_DECL_OVERRIDE;
	void dragMoveEvent(QGraphicsSceneDragDropEvent *event) Q_DECL_OVERRIDE;
	void dragLeaveEvent(QGraphicsSceneDragDropEvent *event) Q_DECL_OVERRIDE;
	void dropEvent(QGraphicsSceneDragDropEvent *event) Q_DECL_OVERRIDE;
private:
	StartSlotData m_data;
	QList<ClassItem*> m_classItems;
	StartSlotHeader *m_header;
	bool m_dragIn = false;
};

}

#endif // DRAWING_STARTSLOTITEM_H
