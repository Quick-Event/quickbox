#include "gridlayoutpropertiesattached.h"

#include <qf/core/log.h>

using namespace qf::qmlwidgets;

GridLayoutPropertiesAttached::GridLayoutPropertiesAttached(QObject *parent) :
	QObject(parent), m_columns(-1), m_rows(-1)
{
}

void GridLayoutPropertiesAttached::setRows(int n)
{
	if(m_rows != n) {
		m_rows = n;
		emit rowsChanged(n);
	}
}

void GridLayoutPropertiesAttached::setColumns(int n)
{
	if(m_columns != n) {
		m_columns = n;
		emit columnsChanged(n);
	}
}


GridLayoutPropertiesAttached *GridLayoutProperties::qmlAttachedProperties(QObject *object)
{
	qfLogFuncFrame() << object;
	return new GridLayoutPropertiesAttached(object);
}
