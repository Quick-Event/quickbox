//
// Author: Frantisek Vacek <fanda.vacek@volny.cz>, (C) 2014
//
// Copyright: See COPYING file that comes with this distribution
//

#include "graphics.h"

#include <qf/core/assert.h>
#include <qf/core/log.h>

#include <QPaintDevice>
#include <QSet>

using namespace qf::qmlwidgets::graphics;

Rect Rect::united(const Rect & _r2) const
{
	Rect r1 = this->normalized();
	Rect r2 = _r2.normalized();
	qreal l = qMin(r1.left(), r2.left());
	qreal r = qMax(r1.right(), r2.right());
	qreal t = qMin(r1.top(), r2.top());
	qreal b = qMax(r1.bottom(), r2.bottom());
	return Rect(l, t, r-l, b-t);
}

qreal qf::qmlwidgets::graphics::x2device(qreal x, QPaintDevice *dev)
{
	QF_ASSERT_EX(dev, "dev is NULL");
	double dpmm = dev->logicalDpiX() / 25.4;
	return x * dpmm;
}

qreal qf::qmlwidgets::graphics::y2device(qreal y, QPaintDevice *dev)
{
	QF_ASSERT_EX(dev, "dev is NULL");
	double dpmm = dev->logicalDpiY() / 25.4;
	return y * dpmm;
}

qreal qf::qmlwidgets::graphics::device2x(qreal x, QPaintDevice *dev)
{
	QF_ASSERT_EX(dev, "dev is NULL");
	double dpmm = dev->logicalDpiX() / 25.4;
	return x / dpmm;
}

qreal qf::qmlwidgets::graphics::device2y(qreal y, QPaintDevice *dev)
{
	QF_ASSERT_EX(dev, "dev is NULL");
	double dpmm = dev->logicalDpiY() / 25.4;
	return y / dpmm;
}

Rect qf::qmlwidgets::graphics::mm2device(const Rect &r, QPaintDevice *dev)
{
	Rect ret;
	ret.setLeft(x2device(r.left(), dev));
	ret.setTop(qf::qmlwidgets::graphics::y2device(r.top(), dev));
	ret.setWidth(qf::qmlwidgets::graphics::x2device(r.width(), dev));
	ret.setHeight(qf::qmlwidgets::graphics::y2device(r.height(), dev));
	return ret;
}

Point qf::qmlwidgets::graphics::mm2device(const Point &p, QPaintDevice *dev)
{
	Point ret;
	ret.setX(qf::qmlwidgets::graphics::x2device(p.x(), dev));
	ret.setY(qf::qmlwidgets::graphics::y2device(p.y(), dev));
	return ret;
}

Point qf::qmlwidgets::graphics::device2mm(const Point &p, QPaintDevice *dev)
{
	QF_ASSERT_EX(dev, "dev is NULL");
	double x_dpmm = dev->logicalDpiX() / 25.4;
	double y_dpmm = dev->logicalDpiY() / 25.4;
	Point ret;
	ret.setX(p.x() / x_dpmm);
	ret.setY(p.y() / y_dpmm);
	return ret;
}

Rect qf::qmlwidgets::graphics::device2mm(const Rect &r, QPaintDevice *dev)
{
	QF_ASSERT_EX(dev, "dev is NULL");
	double x_dpmm = dev->logicalDpiX() / 25.4;
	double y_dpmm = dev->logicalDpiY() / 25.4;
	Rect ret;
	ret.setLeft(r.left() / x_dpmm);
	ret.setTop(r.top() / y_dpmm);
	ret.setWidth(r.width() / x_dpmm);
	ret.setHeight(r.height() / y_dpmm);
	return ret;
}

QList< double > qf::qmlwidgets::graphics::makeLayoutSizes(const QStringList& section_sizes, double layout_size)
{
	QVariantList vlst;
	foreach(QString s, section_sizes)
		vlst << s;
	return qf::qmlwidgets::graphics::makeLayoutSizes(vlst, layout_size);
}

static bool is_absolute_size(const QVariant &v)
{
	//qfLogFuncFrame() << "value:" << v.toString() << "type:" << v.typeName() << "is valid:" << v.isValid();
	bool ret = false;
	//if(v.isValid() && v.type() != QVariant::String)
	v.toString().toDouble(&ret);
	//qfDebug() << "\t return:" << ret;
	return ret;
}

QList< double > qf::qmlwidgets::graphics::makeLayoutSizes(const QVariantList& section_sizes, double layout_size)
{
	qfLogFuncFrame();
	QList<double> ret;
	if(section_sizes.isEmpty())
		return ret;

	for(int i=0; i<section_sizes.count(); i++)
		ret << 0;

	//QVariantList section_sizes = section_sizes;
	double proc_sum = 0;
	int proc_0_cnt = 0;
	QSet<int> relative_ixs;
	qfDebug() << "\t sizes in layout:";
	for(int i=0; i<section_sizes.count(); i++) {
		const QVariant &v = section_sizes[i];
		qfDebug() << "\t\t" << v.toString() << v.typeName() << "invalid:" << !v.isValid();
		if(!is_absolute_size(v)) {
			QString s = v.toString();
			double d = parseRational(s);
			if(d == 0) proc_0_cnt++;
			else if(d > 0) proc_sum += d;
			relative_ixs << i;
			ret[i] = d;
		}
		else {
			ret[i] = v.toDouble();
		}
	}
	/// secti absolutni rozmery
	double abs_sum = 0;
	for(int i=0; i<section_sizes.count(); i++) {
		if(!relative_ixs.contains(i)) {
			abs_sum += ret[i];
		}
	}
	qfDebug() << "\t" << "abs_sum:" << abs_sum << "proc_sum:" << proc_sum << "proc_0_cnt:" << proc_0_cnt << "ly_size:" << layout_size;
	if(layout_size < abs_sum) {
		qfWarning() << "qf::qmlwidgets::graphics::makeLayoutSizes(): Children cann't fit parent. ly_size:" << layout_size << "abs_sum:" << abs_sum;
		layout_size = abs_sum;
	}
	if(proc_sum > 1) {
		qfWarning() << "qf::qmlwidgets::graphics::makeLayoutSizes(): Relative sizes sum exceedes 100%.";
		proc_sum = 1;
	}
	for(int i=0; i<section_sizes.count(); i++) {
		if(relative_ixs.contains(i)) {
			double d = ret[i];
			if(d == 0) {
				if(proc_0_cnt == 0) {
					qfWarning() << "qf::qmlwidgets::graphics::makeLayoutSizes(): 0% count is 0 but 0% dimensions exist.";
				}
				else d = (1 - proc_sum) / proc_0_cnt;
			}
			d = d * (layout_size - abs_sum);
			ret[i] = d;
		}
	}
	return ret;
}



double qf::qmlwidgets::graphics::parseRational(const QString &rational_or_proc_repr)
{
	double d = 0;
	int ix;
	if((ix = rational_or_proc_repr.indexOf('/')) > 0) {
		double d1 = rational_or_proc_repr.mid(0, ix).trimmed().toDouble();
		double d2 = rational_or_proc_repr.mid(ix + 1).trimmed().toDouble();
		if(d1 > 0 && d2 > 0) {
			d = d1 / d2;
		}
	}
	else if((ix = rational_or_proc_repr.indexOf('%')) > 0) {
		d = rational_or_proc_repr.mid(0, ix).trimmed().toDouble();
		d = d / 100;
	}
	else {
		d = rational_or_proc_repr.toDouble();
	}
	return d;
}
