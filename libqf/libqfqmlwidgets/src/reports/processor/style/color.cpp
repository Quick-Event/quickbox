#include "color.h"

#include <qf/core/log.h>

using namespace qf::qmlwidgets::reports::style;

Color::Color(QObject *parent) :
	QObject(parent)
{
	qfLogFuncFrame() << this << "parent:" << parent;
	m_name = nextSequentialName();
}

Color::~Color()
{
	qfLogFuncFrame() << name() << "parent:" << parent();
}

QString Color::nextSequentialName()
{
	static int n = 0;
	return QString("color_%1").arg(++n);
}
