#include "startslotitem.h"
#include "classitem.h"
#include "ganttitem.h"
#include "ganttscene.h"

#include <QGraphicsSceneMouseEvent>
#include <QJsonDocument>
#include <QPainter>
#include <QtGlobal>
#include <QMimeData>

using namespace drawing;

StartSlotItem::StartSlotItem(QGraphicsItem *parent)
	: Super(parent), IGanttItem(this)
{
	m_textSlotNo = new QGraphicsTextItem(this);
	//setFlag(QGraphicsItem::ItemIsSelectable);
	setAcceptDrops(true);
}

int StartSlotItem::classItemCount() const
{
	return m_classItems.count();
}

int StartSlotItem::classItemIndex(const ClassItem *it) const
{
	return m_classItems.indexOf((ClassItem *)it);
}

void StartSlotItem::insertClassItem(int ix, ClassItem *it)
{
	QF_ASSERT(it != nullptr, "Item == NULL", return);
	m_classItems.insert(ix, it);
	it->setParentItem(this);
}

ClassItem *StartSlotItem::classItemAt(int ix, bool throw_ex)
{
	ClassItem *ret = m_classItems.value(ix);
	if(!ret && throw_ex)
		QF_EXCEPTION(QString("Invalid item index %1, item count %2!").arg(ix).arg(classItemCount()));
	return ret;
}

ClassItem *StartSlotItem::takeClassItemAt(int ix)
{
	ClassItem *ret = m_classItems.takeAt(ix);
	ret->setParentItem(nullptr);
	return ret;
}

void StartSlotItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	if(m_dragIn) {
		QColor c("steelblue");
		c.setAlpha(32);
		QRectF r = rect();
		r.setLeft(0);
		painter->fillRect(r, c);
	}
	Super::paint(painter, option, widget);
}

ClassItem *StartSlotItem::addClassItem()
{
	auto *it = new ClassItem(this);
	insertClassItem(classItemCount(), it);
	return it;
}

const StartSlotData& StartSlotItem::data() const
{
	return m_data;
}

void StartSlotItem::setData(const StartSlotData &data)
{
	m_data = data;
}

static constexpr int LABEL_WIDTH_DU = 5;

void StartSlotItem::updateGeometry()
{
	qfLogFuncFrame();
	QRectF r;
	m_textSlotNo->setPlainText(QString::number(slotNumber()));
	int label_width = minToPx(ganttScene()->duToMin(LABEL_WIDTH_DU));
	m_textSlotNo->setPos(-label_width, 0);
	int pos_x = 0;
	double h = m_textSlotNo->boundingRect().height();
	for (int i = 0; i < classItemCount(); ++i) {
		ClassItem *it = classItemAt(i);
		qfDebug() << i << it;
		it->setPos(pos_x, 0);
		it->updateGeometry();
		pos_x += it->rect().width();
		h = qMax(h, it->rect().height());
	}
	r.setHeight(h);
	r.setWidth(label_width + pos_x);
	r.moveLeft(-label_width);
	//qfInfo() << r.left() << r.top() << r.width() << r.height();
	setRect(r);
}

void StartSlotItem::setClassAreaWidth(int px)
{
	QRectF r = rect();
	r.setRight(px);
	setRect(r);
}

void StartSlotItem::dragEnterEvent(QGraphicsSceneDragDropEvent *event)
{
	event->setAccepted(true);
	m_dragIn = true;
	update();
}

void StartSlotItem::dragMoveEvent(QGraphicsSceneDragDropEvent *event)
{
	event->setAccepted(true);
}

void StartSlotItem::dragLeaveEvent(QGraphicsSceneDragDropEvent *event)
{
	Q_UNUSED(event);
	m_dragIn = false;
	update();
}

void StartSlotItem::dropEvent(QGraphicsSceneDragDropEvent *event)
{
	qfLogFuncFrame();
	QJsonDocument jsd = QJsonDocument::fromJson(event->mimeData()->text().toUtf8());
	QVariantMap m = jsd.toVariant().toMap();
	Qt::DropAction act = (m.isEmpty())? Qt::IgnoreAction: Qt::MoveAction;
	event->setDropAction(act);
	event->accept();

	int slot1_ix = m.value(QStringLiteral("slotIndex"), -1).toInt();
	int class1_ix = m.value(QStringLiteral("classIndex"), -1).toInt();
	int slot2_ix = ganttItem()->startSlotItemIndex(this);
	int class2_ix = classItemCount();
	qfDebug() << "DROP class:" << slot1_ix << class1_ix;
	ganttItem()->moveClassItem(slot1_ix, class1_ix, slot2_ix, class2_ix);

	m_dragIn = false;
	update();
}

