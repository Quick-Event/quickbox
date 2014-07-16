#include "layouttypeproperties.h"

#include <qf/core/log.h>

using namespace qf::qmlwidgets;

LayoutTypeProperties::LayoutTypeProperties(QObject *parent) :
	Super(parent), m_columns(-1), m_rows(-1), m_spacing(-1)
{
}

void LayoutTypeProperties::setRows(int n)
{
	if(m_rows != n) {
		m_rows = n;
		emit rowsChanged(n);
	}
}

void LayoutTypeProperties::setColumns(int n)
{
	if(m_columns != n) {
		m_columns = n;
		emit columnsChanged(n);
	}
}

void LayoutTypeProperties::setSpacing(int n)
{
	if(m_spacing != n) {
		m_spacing = n;
		emit spacingChanged(n);
	}
}

LayoutTypeProperties::Flow LayoutTypeProperties::flow()
{
	if(rows() > 0 && columns() <= 0)
		return TopToBottom;
	return LeftToRight;
}



