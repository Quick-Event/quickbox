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
	qfInfo() << this << parent();
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
			Color* pco = color();
			if(pco) {
				QColor c = pco->color();
				m_pen.setColor(c);
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
			m_pen.setStyle((Qt::PenStyle)ps);
		}
	}
    return m_pen;
}
