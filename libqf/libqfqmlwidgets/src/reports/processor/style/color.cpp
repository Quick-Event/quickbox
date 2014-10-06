#include "color.h"

#include <qf/core/log.h>

using namespace qf::qmlwidgets::reports::style;

Color::Color(QObject *parent) :
	QObject(parent)
{
	qfLogFuncFrame() << this << "parent:" << parent;
	setName(nextSequentialName());
	m_dirty = true;
	connect(this, SIGNAL(definitionChanged(QVariant)), this, SLOT(setDirty()));
}

Color::~Color()
{
	qfLogFuncFrame() << this << "parent:" << parent();
}

QColor Color::color()
{
	if(m_dirty) {
		m_dirty = false;
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

QString Color::nextSequentialName()
{
	static int n = 0;
	return QString("color_%1").arg(++n);
}
