#include "punchestableview.h"

#include <qf/core/log.h>

#include <QDrag>
#include <QDragEnterEvent>
#include <QMimeData>
#include <QPainter>
#include <QPixmap>

PunchesTableView::PunchesTableView(QWidget *parent)
	: Super(parent)
{
	setDropIndicatorShown(false);
}

bool PunchesTableView::edit(const QModelIndex &index, QAbstractItemView::EditTrigger trigger, QEvent *event)
{
	Q_UNUSED(event)
	if(trigger == QAbstractItemView::EditTrigger::DoubleClicked
	   || trigger == QAbstractItemView::EditTrigger::EditKeyPressed) {
		qf::core::utils::TableRow row = tableRow(index.row());
		int class_id = row.value("classes.id").toInt();
		int code = row.value("punches.code").toInt();
		qfDebug() << "codeClassActivated:" << class_id << code;
		emit codeClassActivated(class_id, code);
	}
	return false;
}
/*
void PunchesTableView::mousePressEvent(QMouseEvent *event)
{
	qfInfo() << Q_FUNC_INFO;
	QModelIndex ix = indexAt(event->pos());
	if (!ix.isValid())
		return;

	qf::core::utils::TableRow row = tableRow(ix.row());
	QString class_name = row.value(QStringLiteral("classes.name")).toString();
	int code = row.value(QStringLiteral("punches.code")).toInt();

	QByteArray item_data;
	QDataStream data_stream(&item_data, QIODevice::WriteOnly);
	data_stream << ix.row() << ix.column();

	QMimeData *mime_data = new QMimeData;
	mime_data->setData("application/x-quickevent", item_data);

	QDrag *drag = new QDrag(this);
	drag->setMimeData(mime_data);
	//drag->setPixmap(pixmap);
	//drag->setHotSpot(event->pos() - child->pos());

	QPixmap px{QSize{10, 10}};
	QPainter painter;
	QFont f = font();
	QFontMetrics fm(f, &px);
	QString s = QString("%1 - %2").arg(class_name).arg(code);
	QRect bounding_rect = fm.boundingRect(s);
	static constexpr int inset = 5;
	bounding_rect.adjust(-inset, -inset, inset, inset);
	px = QPixmap{bounding_rect.size()};
	painter.begin(&px);
	painter.setFont(f);
	//painter.setPen(Qt::black);
	//painter.setBrush(Qt::black);
	painter.fillRect(px.rect(), QColor("khaki"));
	painter.drawRect(QRect(QPoint(), bounding_rect.size() - QSize(1, 1)));
	painter.drawText(QPoint{inset, inset + fm.ascent()}, s);
	painter.end();

	drag->setPixmap(px);

	if (drag->exec(Qt::CopyAction | Qt::MoveAction, Qt::CopyAction) == Qt::MoveAction) {
		//child->close();
	} else {
		//child->show();
		//child->setPixmap(pixmap);
	}
}

void PunchesTableView::dragEnterEvent(QDragEnterEvent *event)
{
	if (event->mimeData()->hasFormat("application/x-quickevent")) {
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

void PunchesTableView::dragMoveEvent(QDragMoveEvent *event)
{
	if (event->mimeData()->hasFormat("application/x-quickevent")) {
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
*/
