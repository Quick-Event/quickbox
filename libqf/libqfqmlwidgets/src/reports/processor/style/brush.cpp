#include "brush.h"

#include <qf/core/log.h>

using namespace qf::qmlwidgets::reports::style;

Brush::Brush(QObject *parent) :
	StyleObject(parent, StyleObject::SGBrush)
{
}

Brush::~Brush()
{
	//qfLogFuncFrame();
}

QBrush Brush::brush()
{
	qfLogFuncFrame() << this << "is dirty:" << isDirty();
	if(isDirty()) {
        setDirty(false);
        {
			QVariant v = basedOn();
			qfDebug() << "\t based on:" << v.toString();
			if(v.isValid()) {
                QObject *o = styleobjectFromVariant(v);
				Brush *based_on = qobject_cast<Brush*>(o);
				qfDebug() << "\t\t based on object:" << based_on;
                if(based_on) {
                    m_brush = based_on->brush();
                }
			}
		}
		{
			Color* pco = color();
			qfDebug() << "\t color:" << pco;
			if(pco) {
				QColor c = pco->color();
				qfDebug() << "\t\t name:" << c.name();
				m_brush.setColor(c);
			}
		}
		{
			BrushStyle bs = style();
			if(bs != NoBrush) {
				// override any previous definition
				m_brush.setStyle((Qt::BrushStyle)bs);
			}
		}
		if(m_brush.style() == Qt::NoBrush) {
			// is style is undefined, set it to solid pattern
			// solid pattern cannot be default brush definition value,
			// because it can overide any basedOn brush definition then
			m_brush.setStyle(Qt::SolidPattern);
		}
	}
    return m_brush;
}
