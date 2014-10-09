#include "pen.h"
#include "color.h"
#include "sheet.h"
#include "../reportprocessor.h"

#include <qf/core/log.h>

#include <QColor>

using namespace qf::qmlwidgets::reports::style;

Pen::Pen(QObject *parent) :
    QObject(parent), IStyled(this, IStyled::SGPen)
{
	setName(nextSequentialName());
}

Pen::~Pen()
{
    setName(QString());
}

QPen Pen::pen()
{
    if(isDirty()) {
        setDirty(false);
        {
			QVariant v = basedOn();
			if(v.isValid()) {
                QObject *o = styleobjectFromVariant(v);
				Pen *based_on = qobject_cast<Pen*>(o);
                if(based_on) {
                    m_pen = based_on->pen();
                }
			}
		}
		{
			QVariant v = color();
			if(v.isValid()) {
				QColor c;
                QObject *o = styleobjectFromVariant(v, IStyled::SGColor, false);
                Color *pco = qobject_cast<Color*>(o);
                if(pco) {
                    c = pco->color();
                }
                else if(v.type() == QVariant::String) {
					c.setNamedColor(v.toString());
				}
				if(c.isValid()) {
					m_pen.setColor(c);
				}
                else {
                    qfWarning() << "Cannot find color definition for:" << v.toString();
                }
			}
		}
		{
			QVariant v = width();
			if(v.isValid()) {
				m_pen.setWidth(v.toReal());
			}
		}
		{
			PenStyle ps = style();
			if(ps != PenUndefined) {
				m_pen.setStyle((Qt::PenStyle)ps);
			}
		}
	}
    return m_pen;
}
