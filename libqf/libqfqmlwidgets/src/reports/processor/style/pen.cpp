#include "pen.h"

#include <qf/core/log.h>

using namespace qf::qmlwidgets::reports::style;

Pen::Pen(QObject *parent) :
	QObject(parent)
{
	m_name = nextSequentialName();
}

QString Pen::nextSequentialName()
{
	static int n = 0;
	return QString("pen_%1").arg(++n);
}
