#include "classitem.h"
#include "ganttscene.h"
#include "startslotitem.h"

#include <qf/core/sql/query.h>

#include <QApplication>
#include <QDrag>
#include <QJsonDocument>
#include <QMimeData>
#include <QPainter>
#include <QCursor>
#include <QGraphicsSceneMouseEvent>
#include <QWidget>
#include <QStyleOptionGraphicsItem>

using namespace drawing;

ClassData::ClassData(const qf::core::sql::Query &q)
{
	for(auto s : {"id",
		"className", "classId",
		"courseName", "courseId",
		"startSlotIndex", "startSlotPosition",
		"startTimeMin", "startIntervalMin",
		"vacantsBefore", "vacantsBefore", "vacantEvery", "vacantsAfter",
		"firstCode",
		"runsCount"}) {
		insert(s, q.value(s));
	}
}

ClassItem::ClassItem(QGraphicsItem *parent)
	: Super(parent), IGanttItem(this)
{
	int du_px = ganttScene()->displayUnit();
	m_classText = new QGraphicsTextItem(this);
	m_courseText = new QGraphicsTextItem(this);
	m_courseText->setPos(0, 2 * du_px);
	m_classdefsText = new QGraphicsTextItem(this);
	m_classdefsText->setPos(0, 4 * du_px);
	QRect r;
	r.setHeight(6 * du_px + du_px/2);
	setRect(r);

	setAcceptDrops(true);
}

const ClassData &ClassItem::data() const
{
	return m_data;
}

void ClassItem::setData(const ClassData &data)
{
	m_data = data;
}

QColor ClassItem::color() const
{
	auto dt = data();
	int hue = dt.firstCode() % 100;
	QColor c;
	c.setHsvF(hue / 100., 1, 1);
	return c;
}

void ClassItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	QColor c_runner = color();
	QColor c_free = c_runner;
	c_free.setAlpha(64);
	auto r = rect();
	QRectF r1 = r;
	r1.setWidth(minToPx(1));
	r1.setHeight(r.height() / 8);
	painter->save();
	painter->fillRect(r, c_free);
	auto dt = data();
	int interval = dt.startIntervalMin();
	for (int i = 0; i < runsAndVacantCount(); ++i) {
		r1.moveLeft(minToPx(i * interval));
		//QColor c = (i == 0)? c_first: c_runner;
		painter->fillRect(r1, c_runner);
		painter->drawRect(r1);
	}
	if(m_dropInsertsBefore.isValid()) {
		QColor c_insert = c_runner;
		//c_insert.setAlpha(128);
		QRectF r2 = r;
		r2.setWidth(minToPx(2));
		if(!m_dropInsertsBefore.toBool()) {
			r2.moveLeft(r.width() - r2.width());
		}
		painter->fillRect(r2, c_insert);
		QPen p;
		p.setColor(color());
		painter->setPen(p);
		painter->drawRect(r2);
	}
	painter->restore();
	Super::paint(painter, option, widget);
}

int ClassItem::runsAndVacantCount() const
{
	auto dt = data();
	int cnt = dt.runsCount();
	int vacants = (dt.vacantEvery() > 0)? cnt / dt.vacantEvery(): 0;
	cnt = dt.vacantsBefore() + cnt + vacants + dt.vacantsAfter();
	return cnt;
}

int ClassItem::durationMin() const
{
	auto dt = data();
	return runsAndVacantCount() * dt.startIntervalMin();
}

void ClassItem::updateGeometry()
{
	auto dt = data();
	qfLogFuncFrame() << dt.className() << "runners:" << dt.runsCount();
	QRectF r = rect();
	int cnt = dt.runsCount();
	int vacants = (dt.vacantEvery() > 0)? cnt / dt.vacantEvery(): 0;
	cnt = dt.vacantsBefore() + cnt + vacants + dt.vacantsAfter();
	r.setWidth(minToPx(durationMin()));
	setRect(r);
	//setBrush(color());
	m_classText->setHtml(QString("<b>%1</b> %2+%3").arg(dt.className()).arg(dt.runsCount()).arg(runsAndVacantCount() - dt.runsCount()));
	m_courseText->setHtml(QString("<b>%1</b> (%2)").arg(dt.firstCode()).arg(dt.courseId()));
	dt.setStartTimeMin(pxToMin(pos().x()));
	m_classdefsText->setPlainText(QString("%1 / %2").arg(dt.startTimeMin()).arg(dt.startIntervalMin()));
	{
		QString tool_tip;
		// HTML tooltip can cause word wrap
		tool_tip = "<html><body>";
		tool_tip += QString("class: <b>%1</b>, %2 runners + %3 vacants<br/>").arg(dt.className()).arg(dt.runsCount()).arg(runsAndVacantCount() - dt.runsCount());
		tool_tip += QString("first code <b>%1</b>, course name: %2<br/>").arg(dt.firstCode()).arg(dt.courseName());
		tool_tip += QString("vacants before: %1, every: %2, after: %3<br/>").arg(dt.vacantsBefore()).arg(dt.vacantEvery()).arg(dt.vacantsAfter());
		tool_tip += QString("start: %1, duration: %2, end: %3").arg(dt.startTimeMin()).arg(durationMin()).arg(dt.startTimeMin() + durationMin());
		tool_tip += "</body></html>";
		tool_tip.replace(' ', "&nbsp;");
		setToolTip(tool_tip);
	}
}

void ClassItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
	Q_UNUSED(event)
	setCursor(Qt::ClosedHandCursor);
}

void ClassItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
	if (QLineF(event->screenPos(), event->buttonDownScreenPos(Qt::LeftButton)).length() < QApplication::startDragDistance()) {
		return;
	}
	qfLogFuncFrame();
	QDrag *drag = new QDrag(event->widget());
	QMimeData *mime = new QMimeData;
	drag->setMimeData(mime);
	{
		QJsonDocument jsd = QJsonDocument::fromVariant(data());
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
			m_classText->paint(&painter, &opt, 0);
			painter.translate(m_courseText->pos());
			m_courseText->paint(&painter, &opt, 0);
			painter.translate(m_classdefsText->pos() - m_courseText->pos());
			m_classdefsText->paint(&painter, &opt, 0);
		}
		painter.end();
		//pixmap.setMask(pixmap.createHeuristicMask());

		drag->setPixmap(pixmap);
		drag->setHotSpot(QPoint(5 * ganttScene()->displayUnit(), 0));
	}
	Qt::DropAction act = drag->exec();
	qfDebug() << "drag exit:" << act;
	if(act == Qt::MoveAction) {

	}
	setCursor(Qt::OpenHandCursor);
}

void ClassItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
	Q_UNUSED(event)
	setCursor(Qt::OpenHandCursor);
}

void ClassItem::dragEnterEvent(QGraphicsSceneDragDropEvent *event)
{
	event->setAccepted(true);
	QPointF pos = mapFromScene(event->scenePos());
	m_dropInsertsBefore = (pos.x() < rect().width() / 2)? true: false;
	update();
}

void ClassItem::dragMoveEvent(QGraphicsSceneDragDropEvent *event)
{
	event->setAccepted(true);
	QPointF pos = mapFromScene(event->scenePos());
	bool before = (pos.x() < rect().width() / 2)? true: false;
	//qfInfo() << pos.x() << (rect().width() / 2) << before << m_dropInsertsBefore;
	if(before != m_dropInsertsBefore) {
		m_dropInsertsBefore = before;
		update();
	}
}

void ClassItem::dragLeaveEvent(QGraphicsSceneDragDropEvent *event)
{
	Q_UNUSED(event);
	m_dropInsertsBefore = QVariant();
	update();
}

void ClassItem::dropEvent(QGraphicsSceneDragDropEvent *event)
{
	m_dropInsertsBefore = QVariant();
	QJsonDocument jsd = QJsonDocument::fromJson(event->mimeData()->text().toUtf8());
	ClassData dt(jsd.toVariant().toMap());
	Qt::DropAction act = (dt.isEmpty())? Qt::IgnoreAction: Qt::MoveAction;
	event->setDropAction(act);
	event->accept();
	//if (event->mimeData()->hasColor())
	//	color = qvariant_cast<QColor>(event->mimeData()->colorData());
	update();
}



