#include "layoutpropertiesattached.h"

#include <qf/core/log.h>

using namespace qf::qmlwidgets;

LayoutPropertiesAttached::LayoutPropertiesAttached(QObject *parent) :
	QObject(parent)
{
}

void LayoutPropertiesAttached::setBuddyText(const QString &s)
{
	m_formLayoutBuddyText = s;
}


LayoutPropertiesAttached *LayoutProperties::qmlAttachedProperties(QObject *object)
{
	qfLogFuncFrame() << object;
	return new LayoutPropertiesAttached(object);
}
