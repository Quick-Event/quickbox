#include "boxlayouttypeproperties.h"

#include <qf/core/log.h>

using namespace qf::qmlwidgets;

BoxLayoutTypeProperties::BoxLayoutTypeProperties(QObject *parent) :
	Super(parent), m_spacing(-1)
{
}

void BoxLayoutTypeProperties::setSpacing(int n)
{
	if(m_spacing != n) {
		m_spacing = n;
		emit spacingChanged(n);
	}
}

