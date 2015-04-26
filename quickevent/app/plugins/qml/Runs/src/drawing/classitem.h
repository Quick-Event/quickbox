#ifndef DRAWING_CLASSITEM_H
#define DRAWING_CLASSITEM_H

#include "iganttitem.h"

#include <qf/core/utils.h>

#include <QGraphicsRectItem>
#include <QApplication>

namespace qf {
namespace core {
namespace sql {
class Query;
}
}
}
namespace drawing {

class GanttScene;
class StartSlotItem;
class GanttItem;

class ClassData : public QVariantMap
{
public:
	static constexpr int INVALID_START_TIME_SEC = -999999;
private:
	typedef QVariantMap Super;

	QF_VARIANTMAP_FIELD2(int, i, setI, d, 0)
	QF_VARIANTMAP_FIELD2(QString, c, setC, lassName, 0)
	QF_VARIANTMAP_FIELD2(int, c, setC, lassId, 0)
	QF_VARIANTMAP_FIELD2(QString, c, setC, ourseName, 0)
	QF_VARIANTMAP_FIELD2(int, c, setC, ourseId, 0)
	QF_VARIANTMAP_FIELD2(int, s, setS, tartSlotIndex, -1)
	QF_VARIANTMAP_FIELD2(int, c, setC, lassIndex, -1) ///< not SQL value, internal usage
	QF_VARIANTMAP_FIELD2(int, s, setS, tartTimeMin, 0)
	QF_VARIANTMAP_FIELD2(int, s, setS, tartIntervalMin, 0)
	QF_VARIANTMAP_FIELD2(int, v, setV, acantsBefore, 0)
	QF_VARIANTMAP_FIELD2(int, v, setV, acantEvery, 0)
	QF_VARIANTMAP_FIELD2(int, v, setV, acantsAfter, 0)
	QF_VARIANTMAP_FIELD2(int, f, setF, irstCode, 0)
	QF_VARIANTMAP_FIELD2(int, r, setr, unsCount, 0)
	//QF_VARIANTMAP_FIELD2(bool, isD, setD, rawnIn, false) ///< not SQL value, internal usage
	QF_VARIANTMAP_FIELD2(int, m, setM, inStartTimeSec, INVALID_START_TIME_SEC)
	QF_VARIANTMAP_FIELD2(int, m, setM, axStartTimeSec, INVALID_START_TIME_SEC)
public:
	ClassData(const QVariantMap &data = QVariantMap()) : QVariantMap(data) {}
	ClassData(const qf::core::sql::Query &q);

	bool isDrawnIn() const
	{
		return !(minStartTimeSec() == INVALID_START_TIME_SEC && maxStartTimeSec() == INVALID_START_TIME_SEC);
	}
};

class ClassItem : public QGraphicsRectItem, public IGanttItem
{
	Q_DECLARE_TR_FUNCTIONS(drawing::ClassItem)
private:
	typedef QGraphicsRectItem Super;
public:
	ClassItem(QGraphicsItem * parent = 0);

	void updateGeometry();

	const ClassData& data() const;
	void setData(const ClassData &data);

	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0) Q_DECL_OVERRIDE;

	void mousePressEvent(QGraphicsSceneMouseEvent *event) Q_DECL_OVERRIDE;
	void mouseMoveEvent(QGraphicsSceneMouseEvent *event) Q_DECL_OVERRIDE;
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) Q_DECL_OVERRIDE;

	void dragEnterEvent(QGraphicsSceneDragDropEvent *event) Q_DECL_OVERRIDE;
	void dragMoveEvent(QGraphicsSceneDragDropEvent *event) Q_DECL_OVERRIDE;
	void dragLeaveEvent(QGraphicsSceneDragDropEvent *event) Q_DECL_OVERRIDE;
	void dropEvent(QGraphicsSceneDragDropEvent *event) Q_DECL_OVERRIDE;
protected:
	int runsAndVacantCount() const;
	int durationMin() const;
	QColor color() const;
	const StartSlotItem* startSlotItem() const;
	StartSlotItem* startSlotItem();
	//QPair<int, int> ganttIndex() const;
private:
	ClassData m_data;
	QGraphicsTextItem *m_classText;
	QGraphicsTextItem *m_courseText;
	QGraphicsTextItem *m_classdefsText;
	QVariant m_dropInsertsBefore;
	bool m_classClash = false;
};

}

#endif // DRAWING_CLASSITEM_H
