#ifndef DRAWING_STARTSLOTITEM_H
#define DRAWING_STARTSLOTITEM_H

#include <qf/core/utils.h>

#include <QGraphicsRectItem>

namespace drawing {

class ClassItem;

class StartSlotData : public QVariantMap
{
public:
	explicit StartSlotData(const QVariantMap &m = QVariantMap()) : QVariantMap(m) {}

	QF_VARIANTMAP_FIELD2(int, s, setS, tart, 0)
	QF_VARIANTMAP_FIELD2(bool, is, set, Locked, false)
};

class StartSlotItem : public QGraphicsRectItem
{
private:
	typedef QGraphicsRectItem Super;
public:
	StartSlotItem(QGraphicsItem * parent = 0);

	ClassItem* addClassItem();
	int classItemCount();
	ClassItem *classItem(int ix);

	const StartSlotData& data() const;
	void setData(const StartSlotData &data);
private:
	StartSlotData m_data;
	QList<ClassItem*> m_classItems;
};

}

#endif // DRAWING_STARTSLOTITEM_H
