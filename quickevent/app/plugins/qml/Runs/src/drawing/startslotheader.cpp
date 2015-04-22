#include "startslotheader.h"
#include "startslotitem.h"
#include "ganttscene.h"

#include <QGraphicsSceneMouseEvent>
#include <QMenu>
#include <QInputDialog>

using namespace drawing;

StartSlotHeader::StartSlotHeader(QGraphicsItem *parent)
	: Super(parent), IGanttItem(this)
{
	m_textSlotNo = new QGraphicsTextItem(this);
}

int StartSlotHeader::minHeight()
{
	return m_textSlotNo->boundingRect().height();
}

static constexpr int LABEL_WIDTH_DU = 5;

void StartSlotHeader::updateGeometry()
{
	m_textSlotNo->setPlainText(QString::number(startSlotItem()->slotNumber()));
	int label_width = minToPx(ganttScene()->duToMin(LABEL_WIDTH_DU));
	QRectF r = startSlotItem()->rect();
	r.setWidth(label_width);
	setRect(r);
	setPos(-label_width, 0);
}

void StartSlotHeader::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
	StartSlotData dt = startSlotItem()->data();
	QMenu menu;
	QAction *a_set_start = menu.addAction(tr("Set slot start offset"));
	QAction *a_locked = menu.addAction(tr("Locked"));
	a_locked->setCheckable(true);
	a_locked->setChecked(dt.isLocked());
	QAction *a = menu.exec(event->screenPos());
	if(a == a_set_start) {
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
			dt.setStartOffset(i);
			startSlotItem()->setData(dt);
			startSlotItem()->updateGeometry();
		}
	}
	else if(a == a_locked) {
		dt.setLocked(a_locked->isChecked());
		startSlotItem()->setData(dt);
	}
}

StartSlotItem *StartSlotHeader::startSlotItem()
{
	StartSlotItem *ret = dynamic_cast<StartSlotItem*>(parentItem());
	QF_ASSERT_EX(ret != nullptr, "Bad parent!");
	return ret;
}


