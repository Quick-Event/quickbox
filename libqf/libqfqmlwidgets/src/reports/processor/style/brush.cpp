#include "brush.h"

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
	if(isDirty()) {
        setDirty(false);
        {
			QVariant v = basedOn();
			if(v.isValid()) {
                QObject *o = styleobjectFromVariant(v);
				Brush *based_on = qobject_cast<Brush*>(o);
                if(based_on) {
                    m_brush = based_on->brush();
                }
			}
		}
		{
			Color* pco = color();
			if(pco) {
				QColor c = pco->color();
				m_brush.setColor(c);
			}
		}
		{
			BrushStyle bs = style();
			m_brush.setStyle((Qt::BrushStyle)bs);
		}
	}
    return m_brush;
}
