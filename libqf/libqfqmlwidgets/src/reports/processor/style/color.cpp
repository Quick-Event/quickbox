#include "color.h"

#include <qf/core/log.h>

using namespace qf::qmlwidgets::reports::style;

Color::Color(QObject *parent) :
	QObject(parent)
{
	qfLogFuncFrame() << this << "parent:" << parent;
}

Color::~Color()
{
	qfLogFuncFrame() << name() << "parent:" << parent();
}
