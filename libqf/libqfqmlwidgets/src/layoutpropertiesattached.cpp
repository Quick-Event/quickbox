#include "layoutpropertiesattached.h"

#include <qf/core/log.h>

using namespace qf::qmlwidgets;

LayoutPropertiesAttached::LayoutPropertiesAttached(QObject *parent) :
	QObject(parent)
{
	m_rowSpan = 1;
}

void LayoutPropertiesAttached::setRowSpan(int n)
{
	if(n > 0 && n != m_rowSpan) {
		m_rowSpan = n;
		emit rowSpanChanged();
	}
}


LayoutPropertiesAttached *LayoutProperties::qmlAttachedProperties(QObject *object)
{
	qfLogFuncFrame() << object;
	return new LayoutPropertiesAttached(object);
}
