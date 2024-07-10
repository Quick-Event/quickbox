#ifndef DRAWING_GANTTSCENE_H
#define DRAWING_GANTTSCENE_H

#include <QGraphicsScene>

namespace drawing {

class GanttItem;

class GanttScene : public QGraphicsScene
{
	Q_OBJECT
private:
	typedef QGraphicsScene Super;
public:
	GanttScene(QObject * parent = 0);

	void load(int stage_id);
	void save();

	GanttItem *ganttItem() { return m_ganttItem; }

	/**
	 * @brief displayUnit
	 * @return default font line spacing / 2
	 */
	int displayUnit() const {return m_displayUnit;}
	void setDisplayUnit(int display_unit) {m_displayUnit = display_unit;}

	int pxToMin(int px) const;
	int minToPx(int min) const;
	int duToMin(int n) const;

	bool isUseAllMaps() const {return m_useAllMaps;}
private:
	int m_stageId = -1;
	int m_displayUnit;
	GanttItem *m_ganttItem = nullptr;
	bool m_useAllMaps = false;
};

}

#endif
