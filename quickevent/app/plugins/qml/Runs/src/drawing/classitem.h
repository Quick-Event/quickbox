#ifndef DRAWING_CLASSITEM_H
#define DRAWING_CLASSITEM_H

#include <QGraphicsRectItem>

namespace drawing {

class ClassItem : public QGraphicsRectItem
{
private:
	typedef QGraphicsRectItem Super;
public:
	ClassItem(QGraphicsItem * parent = 0);
};

}

#endif // DRAWING_CLASSITEM_H
