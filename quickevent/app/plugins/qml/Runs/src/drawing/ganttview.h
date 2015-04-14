#ifndef GANTTVIEW_H
#define GANTTVIEW_H

#include <QGraphicsView>

namespace drawing {

class GanttView : public QGraphicsView
{
	Q_OBJECT
private:
	typedef QGraphicsView Super;
public:
	GanttView(QWidget *parent = 0);
};

}

#endif // GANTTVIEW_H
