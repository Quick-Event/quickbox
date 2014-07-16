#include "gridlayouttypeproperties.h"

#include <qf/core/log.h>

using namespace qf::qmlwidgets;

GridLayoutTypeProperties::GridLayoutTypeProperties(QObject *parent) :
	Super(parent), m_columns(-1), m_rows(-1)
{
}

void GridLayoutTypeProperties::setRows(int n)
{
	if(m_rows != n) {
		m_rows = n;
		emit rowsChanged(n);
	}
}

void GridLayoutTypeProperties::setColumns(int n)
{
	if(m_columns != n) {
		m_columns = n;
		emit columnsChanged(n);
	}
}

GridLayoutTypeProperties::Flow GridLayoutTypeProperties::flow()
{
	if(rows() > 0 && columns() <= 0)
		return TopToBottom;
	return LeftToRight;
}


