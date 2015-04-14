#ifndef GANTTSCENE_H
#define GANTTSCENE_H

#include <QGraphicsScene>

namespace drawing {

class GanttScene : public QGraphicsScene
{
	Q_OBJECT
private:
	typedef QGraphicsScene Super;
public:
	GanttScene(QObject * parent = 0);
};

}

#endif // GANTTSCENE_H
