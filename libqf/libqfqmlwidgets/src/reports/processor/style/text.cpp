#include "pen.h"
#include "color.h"
#include "sheet.h"
#include "../reportprocessor.h"

#include <qf/core/log.h>

#include <QColor>

using namespace qf::qmlwidgets::reports::style;

Text::Text(QObject *parent) :
    StyleObject(parent, StyleObject::SGText)
{
}

Text::~Text()
{
    setName(QString());
}

CompiledTextStyle Text::textStyle()
{
    if(isDirty()) {
        setDirty(false);
        {
			QVariant v = basedOn();
			if(v.isValid()) {
                QObject *o = styleobjectFromVariant(v);
				Text *based_on = qobject_cast<Text*>(o);
                if(based_on) {
                    m_textStyle = based_on->textStyle();
                }
			}
		}
		{
			Pen* p = pen();
			if(p) {
				m_textStyle.setPen(p->pen());
			}
		}
		{
			Font* p = font();
			if(p) {
				m_textStyle.setFont(p->font());
			}
		}
	}
    return m_textStyle;
}
