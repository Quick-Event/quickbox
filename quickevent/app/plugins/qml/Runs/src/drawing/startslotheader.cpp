#include "startslotheader.h"
#include "startslotitem.h"
#include "ganttscene.h"
#include "ganttitem.h"

#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneWheelEvent>
#include <QMenu>
#include <QInputDialog>
#include <QPainter>
#include <QApplication>
#include <QDrag>
#include <QMimeData>
#include <QStyleOptionGraphicsItem>
#include <QJsonDocument>

using namespace drawing;

class LockItem : public QGraphicsRectItem
{
	Q_DECLARE_TR_FUNCTIONS(drawing::ClassdefsLockItem)
public:
	LockItem(StartSlotHeader *parent = 0) : QGraphicsRectItem(parent), m_startSlotItem(parent->startSlotItem())
	{
		int du_px = m_startSlotItem->ganttScene()->displayUnit();
		setRect(0, 0, 3 * du_px, 2 * du_px);
		setToolTip(tr("Lock class start time"));
	}

	void mousePressEvent(QGraphicsSceneMouseEvent *event) Q_DECL_OVERRIDE
	{
		if(event->button() == Qt::LeftButton) {
			m_startSlotItem->setLocked(!m_startSlotItem->isLocked());
			update();
			event->accept();
		}
	}

	void paint(QPainter *painter, const QStyleOptionGraphicsItem * option, QWidget *widget = 0) Q_DECL_OVERRIDE
	{
		Q_UNUSED(option)
		Q_UNUSED(widget)
		//QGraphicsRectItem::paint(painter, option, widget);
		QRectF r = rect();
		QRectF r1(0, 0, r.width() / 3, r.height() / 2);
		QRectF r2(0, 0, r.width() * 2 / 3, r.height() / 2);
		QColor c;
		if(m_startSlotItem->isLocked()) {
			r1.moveLeft(r.width() / 3);
			r2.moveLeft(r.width() / 6);
			c = Qt::red;
		}
		else {
			r1.moveLeft(r.width() / 2);
			c = Qt::blue;
		}
		r1.moveTop(r.height() / 8);
		r2.moveTop(r.height() / 2);

		QPen p(Qt::SolidLine);
		p.setWidthF(r.height() / 8);
		p.setColor(c);
		p.setCapStyle(Qt::FlatCap);
		painter->setPen(p);
		//painter->fillRect(r1, Qt::yellow);
		painter->drawArc(r1, 0, 180 * 16);
		painter->drawLine(r1.bottomLeft(), QPointF(r1.left(), r1.center().y()));
		painter->drawLine(r1.bottomRight(), QPointF(r1.right(), r1.center().y()));

		double d = p.widthF();
		r2.adjust(d, 0, -d, 0);
		painter->fillRect(r2, c);
	}
private:
	StartSlotItem *m_startSlotItem;
};

class StartOffsetTextItem : public QGraphicsTextItem
{
	Q_DECLARE_TR_FUNCTIONS(drawing::StartSlotHeader)
public:
	StartOffsetTextItem(StartSlotHeader * parent = 0) : QGraphicsTextItem(parent), m_header(parent)
	{
		setToolTip(tr("Use mouse wheel to change start slot offset"));
	}

	void wheelEvent(QGraphicsSceneWheelEvent *event) Q_DECL_OVERRIDE
	{
		double n = event->delta() / 120.;
		if(n > -1 && n <= 0)
			n = -1;
		else if(n < 1 && n > 0)
			n = 1;
		m_header->startSlotItem()->setStartOffset(m_header->startSlotItem()->startOffset() + (int)n);
		event->accept();
	}
private:
	StartSlotHeader *m_header;
};

StartSlotHeader::StartSlotHeader(StartSlotItem *parent)
	: Super(parent), IGanttItem(this)
{
	int du_px = ganttScene()->displayUnit();
	m_textSlotNo = new QGraphicsTextItem(this);
	m_lockItem = new LockItem(this);
	m_lockItem->setPos(0, 2 * du_px);
	m_textStartOffset = new StartOffsetTextItem(this);
	m_textStartOffset->setPos(m_lockItem->rect().width(), 2 * du_px);

	setCursor(Qt::ArrowCursor);
	setAcceptDrops(true);
}

int StartSlotHeader::minHeight()
{
	int du_px = ganttScene()->displayUnit();
	return 5 * du_px;
}

static constexpr int LABEL_WIDTH_DU = 6;

void StartSlotHeader::updateGeometry()
{
	m_textSlotNo->setPlainText(QString::number(startSlotItem()->slotNumber()));
	int label_width = minToPx(ganttScene()->duToMin(LABEL_WIDTH_DU));
	QRectF r = startSlotItem()->rect();
	r.setWidth(label_width);
	setRect(r);
	setPos(-label_width, 0);
	m_textStartOffset->setPlainText(QString::number(startSlotItem()->data().startOffset()));
}

void StartSlotHeader::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	const StartSlotData &dt = startSlotItem()->data();
	if(dt.isIgnoreClassClashCheck()) {
		QColor c("khaki");
		c.setAlpha(128);
		QRectF r = rect();
		r.setLeft(0);
		painter->fillRect(r, c);
	}
	if(m_dragIn) {
		QColor c("steelblue");
		c.setAlpha(32);
		QRectF r = rect();
		r.setLeft(0);
		painter->fillRect(r, c);
	}
	Super::paint(painter, option, widget);
}

void StartSlotHeader::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
	StartSlotData dt = startSlotItem()->data();
	QMenu menu;
	QAction *a_append_start_slot = menu.addAction(tr("Append start slot"));
	QAction *a_set_start = menu.addAction(tr("Set slot start offset"));
	QAction *a_locked = menu.addAction(tr("Locked"));
	a_locked->setChecked(dt.isLocked());
	a_locked->setCheckable(true);
	QAction *a_clash = menu.addAction(tr("Ignore class clash check"));
	a_clash->setCheckable(true);
	a_clash->setChecked(dt.isIgnoreClassClashCheck());
	QAction *a = menu.exec(event->screenPos());
	if(a == a_append_start_slot) {
		auto *gi = ganttItem();
		gi->insertStartSlotItem(gi->startSlotItemIndex(startSlotItem()) + 1, new StartSlotItem(gi));
		gi->updateGeometry();
	}
	else if(a == a_set_start) {
		QWidget *w = nullptr;
		QObject *o = scene();
		while(o) {
			w = qobject_cast<QWidget*>(o);
			if(w)
				break;
			o = o->parent();
		}
		bool ok;
		int i = QInputDialog::getInt(w, tr("InputDialog"), tr("Start slot offset [min]:"), dt.startOffset(), 0, 1000000, 1, &ok);
		if(ok) {
			startSlotItem()->setStartOffset(i);
		}
	}
	else if(a == a_clash) {
		dt.setIgnoreClassClashCheck(a_clash->isChecked());
		startSlotItem()->setData(dt);
		update();
	}
	else if(a == a_locked) {
		startSlotItem()->setLocked(!startSlotItem()->isLocked());
		update();
	}
}

StartSlotItem *StartSlotHeader::startSlotItem()
{
	StartSlotItem *ret = dynamic_cast<StartSlotItem*>(parentItem());
	QF_ASSERT_EX(ret != nullptr, "Bad parent!");
	return ret;
}

void StartSlotHeader::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
	Q_UNUSED(event)
	setCursor(Qt::ClosedHandCursor);
}

void StartSlotHeader::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
	if (QLineF(event->screenPos(), event->buttonDownScreenPos(Qt::LeftButton)).length() < QApplication::startDragDistance()) {
		return;
	}
	qfLogFuncFrame();
	QDrag *drag = new QDrag(event->widget());
	QMimeData *mime = new QMimeData;
	drag->setMimeData(mime);
	{
		QVariantMap m;
		int slot_ix = ganttItem()->startSlotItemIndex(startSlotItem());
		m[QStringLiteral("slotIndex")] = slot_ix;
		QJsonDocument jsd = QJsonDocument::fromVariant(m);
		QString mime_text = QString::fromUtf8(jsd.toJson());
		qfDebug() << "mime:" << mime_text;
		mime->setText(mime_text);

		QPixmap pixmap(rect().size().toSize());
		pixmap.fill(Qt::white);

		QPainter painter(&pixmap);
		//painter.translate(15, 15);
		painter.setRenderHint(QPainter::Antialiasing);
		QStyleOptionGraphicsItem opt;
		paint(&painter, &opt, 0);
		{
			m_textSlotNo->paint(&painter, &opt, 0);
			painter.translate(m_textStartOffset->pos());
			m_textStartOffset->paint(&painter, &opt, 0);
		}
		painter.end();
		//pixmap.setMask(pixmap.createHeuristicMask());

		drag->setPixmap(pixmap);
		drag->setHotSpot(QPoint(ganttScene()->displayUnit(), 0));
	}
	Qt::DropAction act = drag->exec();
	qfDebug() << "drag exit:" << act;
	setCursor(Qt::ArrowCursor);
}

void StartSlotHeader::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
	Q_UNUSED(event)
	setCursor(Qt::ArrowCursor);
}

void StartSlotHeader::dragEnterEvent(QGraphicsSceneDragDropEvent *event)
{
	event->setAccepted(true);
	m_dragIn = true;
	update();
}

void StartSlotHeader::dragMoveEvent(QGraphicsSceneDragDropEvent *event)
{
	event->setAccepted(true);
}

void StartSlotHeader::dragLeaveEvent(QGraphicsSceneDragDropEvent *event)
{
	Q_UNUSED(event);
	m_dragIn = false;
	update();
}

void StartSlotHeader::dropEvent(QGraphicsSceneDragDropEvent *event)
{
	qfLogFuncFrame();
	QJsonDocument jsd = QJsonDocument::fromJson(event->mimeData()->text().toUtf8());
	QVariantMap m = jsd.toVariant().toMap();
	Qt::DropAction act = (m.isEmpty())? Qt::IgnoreAction: Qt::MoveAction;
	event->setDropAction(act);
	event->accept();

	int slot1_ix = m.value(QStringLiteral("slotIndex"), -1).toInt();
	int slot2_ix = ganttItem()->startSlotItemIndex(startSlotItem());
	qfDebug() << "DROP header:" << slot1_ix << slot2_ix;
	ganttItem()->moveStartSlotItem(slot1_ix, slot2_ix);

	m_dragIn = false;
	update();
}
