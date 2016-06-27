
//
// Author: Frantisek Vacek <fanda.vacek@volny.cz>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef QF_QMLWIDGETS_GRAPHICS_GRAPHICS_H
#define QF_QMLWIDGETS_GRAPHICS_GRAPHICS_H

#include "../qmlwidgetsglobal.h"

#include <QRectF>
#include <QString>
#include <QVariant>

class QPaintDevice;
//class QVariantList;
namespace qf {
namespace qmlwidgets {
namespace graphics {

enum Layout {LayoutInvalid = 0, LayoutHorizontal = 1, LayoutVertical, LayoutStacked};
class Point : public QPointF
{
public:
	Point() : QPointF(0, 0) {}
	Point(qreal x, qreal y) : QPointF(x, y) {}
	Point(const QPointF &p) : QPointF(p) {}
	//bool isValid() const {return x() >= 0 && y() >= 0;}

	QString toString() const {return QString("point[%1, %2]").arg(x()).arg(y());}
};
class Size : public QSizeF
{
public:
	Size() : QSizeF(0, 0) {}
	Size(qreal x, qreal y) : QSizeF(x, y) {}
	Size(const QSizeF &s) : QSizeF(s) {}

	Size intersect(const Size &sz) {
		Size ret;
		ret.setWidth(qMin(width(), sz.width()));
		ret.setHeight(qMin(height(), sz.height()));
		return ret;
	}

	//! vraci true, kdyz je this ve vsech smerech vetsi nebo rovno sz.
	bool isOverlapping(const Size &sz) {
		return (width() >= sz.width() && height() >= sz.height());
	}

	QString toString() const {return QString("size[%1, %2]").arg(width()).arg(height());}
};
class Rect : public QRectF
{
public:
	Rect() : QRectF() {}
	//Rect(qreal x, qreal y) : QRectF(x, y) {init();}
	Rect(const QPointF &topLeft, const QSizeF &size) : QRectF(topLeft, size) {}
	Rect(qreal x, qreal y, qreal width, qreal height) : QRectF(x, y, width, height) {}
	Rect(const QRectF &r) : QRectF(r) {}
public:
	QString toString() const {
		return QString("rect(%1, %2, size[%3, %4])")
				.arg(Point(topLeft()).toString(), Point(bottomRight()).toString())
				.arg(width()).arg(height());
	}
	Rect transposed() const { return Rect(topLeft(), Size(height(), width())); }
	/// QRectF ma ve verzi 4.5 united pro rect s nulovou sirkou nebo vyskou blbe
	Rect united(const Rect &r) const;
	qreal area() const { return width() * height(); }
};

//! prepocite mm na body vystupniho device
qreal x2device(qreal x, QPaintDevice *dev);
qreal y2device(qreal y, QPaintDevice *dev);
qreal device2x(qreal x, QPaintDevice *dev);
qreal device2y(qreal y, QPaintDevice *dev);
Rect mm2device(const Rect &r, QPaintDevice *dev);
Point mm2device(const Point &p, QPaintDevice *dev);
Point device2mm(const Point &p, QPaintDevice *dev);
Rect device2mm(const Rect &p, QPaintDevice *dev);

double parseRational(const QString &rational_or_proc_repr);

//double stringToProc(const QString &s);
QList<double> makeLayoutSizes(const QStringList &section_sizes, double layout_size);
QList<double> makeLayoutSizes(const QVariantList &section_sizes, double layout_size);

}}}

#endif // QF_QMLWIDGETS_GRAPHICS_GRAPHICS_H

