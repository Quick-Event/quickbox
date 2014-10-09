#include "color.h"

#include <qf/core/log.h>

using namespace qf::qmlwidgets::reports::style;

Color::Color(QObject *parent) :
    QObject(parent), IStyled(this, IStyled::SGColor)
{
	qfLogFuncFrame() << this << "parent:" << parent;
	setName(nextSequentialName());
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
		if(v.type() == QVariant::String) {
			m_color.setNamedColor(v.toString());
		}
		else {
			m_color = v.value<QColor>();
		}
	}
	return m_color;
}
