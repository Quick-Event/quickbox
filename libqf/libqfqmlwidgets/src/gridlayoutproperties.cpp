#include "gridlayoutproperties.h"

#include <qf/core/log.h>

using namespace qf::qmlwidgets;

GridLayoutProperties::GridLayoutProperties(QObject *parent) :
	QObject(parent), m_columns(-1), m_rows(-1)
{
}

void GridLayoutProperties::setRows(int n)
{
	if(m_rows != n) {
		m_rows = n;
		emit rowsChanged(n);
	}
}

void GridLayoutProperties::setColumns(int n)
{
	if(m_columns != n) {
		m_columns = n;
		emit columnsChanged(n);
	}
}

GridLayoutProperties::Flow GridLayoutProperties::flow()
{
	if(rows() > 0 && columns() <= 0)
		return TopToBottom;
	return LeftToRight;
}


