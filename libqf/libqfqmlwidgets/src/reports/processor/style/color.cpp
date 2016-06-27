#include "color.h"
#include "sheet.h"

#include <qf/core/log.h>

using namespace qf::qmlwidgets::reports::style;

Color::Color(QObject *parent) :
	StyleObject(parent, StyleObject::SGColor)
{
	qfLogFuncFrame() << this << "parent:" << parent;
	connect(this, SIGNAL(definitionChanged(QVariant)), this, SLOT(setDirty()));
}

Color::~Color()
{
	qfLogFuncFrame() << this << "parent:" << parent();
	setName(QString());
}

QColor Color::color()
{
	if(isDirty()) {
		setDirty(false);
		QVariant v = definition();
		//qfInfo() << v.toString() << "type:" << v.typeName();
		if(v.type() == QVariant::String) {
			m_color.setNamedColor(v.toString());
			if(!m_color.isValid()) {
				Sheet *ss = rootStyleSheet();
				if(ss) {
					Color *pco = qobject_cast<Color*>(ss->styleObjectForName(SGColor, v.toString()));
					if(pco)
						m_color = pco->color();
				}
			}
		}
		else if(v.type() == QVariant::Int) {
			m_color = QColor((Qt::GlobalColor)v.toInt());
		}
		else {
			m_color = v.value<QColor>();
		}
	}
	return m_color;
}
