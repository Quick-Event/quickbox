#include "runstableview.h"

#include <QDragEnterEvent>
#include <QMimeData>

#include <quickevent/og/timems.h>

RunsTableView::RunsTableView(QWidget *parent)
	: Super(parent)
{
	setSelectionMode(SingleSelection);
	setDragEnabled(true);
	viewport()->setAcceptDrops(true);
	setDropIndicatorShown(true);
	setDragDropMode(QAbstractItemView::InternalMove);
}

RunsTableView::~RunsTableView()
{
}
/*
void RunsTableView::dragEnterEvent(QDragEnterEvent *event)
{
	if (event->mimeData()->hasText()) {
		if (event->source() == this) {
			event->setDropAction(Qt::MoveAction);
			event->accept();
		} else {
			event->acceptProposedAction();
		}
	} else {
		event->ignore();
	}
}

void RunsTableView::dragMoveEvent(QDragMoveEvent *event)
{
	if (event->mimeData()->hasText()) {
		if (event->source() == this) {
			event->setDropAction(Qt::MoveAction);
			event->accept();
		} else {
			event->acceptProposedAction();
		}
	} else {
		event->ignore();
	}
}

void RunsTableView::dropEvent(QDropEvent *event)
{
	if (event->mimeData()->hasText()) {
		QString s = event->mimeData()->text();
		int msec = s.toInt();

		if (event->source() == this) {
			event->setDropAction(Qt::MoveAction);
			event->accept();
		} else {
			event->acceptProposedAction();
		}
	} else {
		event->ignore();
	}
}

void RunsTableView::mousePressEvent(QMouseEvent *event)
{
	Q_UNUSED(event)
	setCursor(Qt::ClosedHandCursor);
}

void RunsTableView::mouseMoveEvent(QMouseEvent *event)
{
	if (QLineF(event->screenPos(), event->buttonDownScreenPos(Qt::LeftButton)).length() < QApplication::startDragDistance()) {
		return;
	}
	QModelIndex index = indexAt(event->pos());
	if (!index.isValid())
		return;

	QMimeData *mimeData = new QMimeData;
	QVariant v = index.data(qf::core::model::TableModel::RawValueRole);
	quickevent::og::TimeMs t = v.value<quickevent::og::TimeMs>();
	mimeData->setText(QString::number(t.msec()));

	QDrag *drag = new QDrag(this);
	drag->setMimeData(mimeData);
	drag->setPixmap(pixmap);
	drag->setHotSpot(event->pos() - child->pos());

	QPixmap tempPixmap = pixmap;
	QPainter painter;
	painter.begin(&tempPixmap);
	painter.fillRect(pixmap.rect(), QColor(127, 127, 127, 127));
	painter.end();

	child->setPixmap(tempPixmap);

	if (drag->exec(Qt::CopyAction | Qt::MoveAction, Qt::CopyAction) == Qt::MoveAction) {
		child->close();
	} else {
		child->show();
		child->setPixmap(pixmap);
	}
}
*/
