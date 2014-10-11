#include "font.h"

using namespace qf::qmlwidgets::reports::style;

Font::Font(QObject *parent) :
	StyleObject(parent, StyleObject::SGFont)
{
}

Font::~Font()
{
	//qfLogFuncFrame();
}

QFont Font::font()
{
	if(isDirty()) {
        setDirty(false);
        {
			QVariant v = basedOn();
			if(v.isValid()) {
                QObject *o = styleobjectFromVariant(v);
				Font *based_on = qobject_cast<Font*>(o);
                if(based_on) {
                    m_font = based_on->font();
                }
			}
		}
		{
			FontStyle bs = style();
			m_font.setStyle((QFont::Style)bs);
		}
		{
			FontStyleHint bs = hint();
			m_font.setStyleHint((QFont::StyleHint)bs);
		}
		{
			FontWeight bs = weight();
			m_font.setWeight((QFont::Weight)bs);
		}
		{
			qreal sz = pointSize();
			if(sz > 0)
				m_font.setPointSizeF(sz);
		}
	}
    return m_font;
}
