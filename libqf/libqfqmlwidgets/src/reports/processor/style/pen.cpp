#include "pen.h"
#include "color.h"
#include "sheet.h"
#include "../reportprocessor.h"

#include <qf/core/log.h>

#include <QColor>

using namespace qf::qmlwidgets::reports::style;

Pen::Pen(QObject *parent) :
	StyleObject(parent, StyleObject::SGPen)
{
}

Pen::~Pen()
{
	setName(QString());
}

QPen Pen::pen()
{
	qfLogFuncFrame() << this << "is dirty:" << isDirty();
	if(isDirty()) {
		setDirty(false);
		{
			QVariant v = basedOn();
			qfDebug() << "\t based on:" << v.toString();
			if(v.isValid()) {
				QObject *o = styleobjectFromVariant(v);
				Pen *based_on = qobject_cast<Pen*>(o);
				qfDebug() << "\t\t based on object:" << based_on;
				if(based_on) {
					m_pen = based_on->pen();
				}
			}
		}
		{
			Color* pco = color();
			qfDebug() << "\t color:" << pco;
			if(pco) {
				QColor c = pco->color();
				qfDebug() << "\t\t name:" << c.name();
				m_pen.setColor(c);
			}
		}
		{
			qreal d = width();
			//qfDebug() << "\t new width:" << d << "dist:" << qFloatDistance(d, 0);
			if(d > 0.000001) {
				//qfDebug() << "\t\t setting new width:" << d;
				m_pen.setWidthF(d);
			}
		}
		{
			PenStyle ps = style();
			if(ps != NoPen)
				m_pen.setStyle((Qt::PenStyle)ps);
		}
		if(m_pen.style() == Qt::NoPen) {
			// if style is undefined, set it to solid line
			// solid line cannot be default pen definition value,
			// because it can overide any basedOn pen definition then
			m_pen.setStyle(Qt::SolidLine);
		}
	}
	m_pen.setCapStyle(Qt::FlatCap);
	qfDebug() << "return width:" << m_pen.widthF() << "style:" << m_pen.style() << "color:" << m_pen.color().name();
	return m_pen;
}
