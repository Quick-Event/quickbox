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

	void load();

	int displayUnit() const {return m_displayUnit;}
	void setDisplayUnit(int display_unit) {m_displayUnit = display_unit;}

private:
	int m_displayUnit;
};

}

#endif // GANTTSCENE_H
