#ifndef DRAWING_CLASSITEM_H
#define DRAWING_CLASSITEM_H

#include <qf/core/utils.h>

#include <QGraphicsRectItem>

namespace qf {
namespace core {
namespace sql {
class Query;
}
}
}
namespace drawing {

class ClassData : public QVariantMap
{
private:
	typedef QVariantMap Super;

	QF_VARIANTMAP_FIELD2(int, i, setI, d, 0)
	QF_VARIANTMAP_FIELD2(int, c, setC, ourseId, 0)
	QF_VARIANTMAP_FIELD2(int, s, setS, tartSlotIndex, -1)
	QF_VARIANTMAP_FIELD2(int, s, setS, tartSlotPosition, -1)
	QF_VARIANTMAP_FIELD2(int, s, setS, tartTimeMin, 0)
	QF_VARIANTMAP_FIELD2(int, s, setS, tartIntervalMin, 0)
	QF_VARIANTMAP_FIELD2(int, v, setV, acantsBefore, 0)
	QF_VARIANTMAP_FIELD2(int, v, setV, acantEvery, 0)
	QF_VARIANTMAP_FIELD2(int, v, setV, vacantsAfter, 0)
public:
	ClassData(const QVariantMap &data = QVariantMap()) : QVariantMap(data) {}
	ClassData(const qf::core::sql::Query &q);
};

class ClassItem : public QGraphicsRectItem
{
private:
	typedef QGraphicsRectItem Super;
public:
	ClassItem(QGraphicsItem * parent = 0);

	const ClassData& data() const;
	void setData(const ClassData &data);

private:
	ClassData m_data;
};

}

#endif // DRAWING_CLASSITEM_H
