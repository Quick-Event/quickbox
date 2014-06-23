#ifndef QF_QMLWIDGETS_GRIDLAYOUTPROPERTIESATTACHED_H
#define QF_QMLWIDGETS_GRIDLAYOUTPROPERTIESATTACHED_H

#include "qmlwidgetsglobal.h"

#include <QObject>
#include <qqml.h>

namespace qf {
namespace qmlwidgets {

class QFQMLWIDGETS_DECL_EXPORT GridLayoutProperties : public QObject
{
	Q_OBJECT
	Q_PROPERTY(int columns READ columns WRITE setColumns NOTIFY columnsChanged)
	Q_PROPERTY(int rows READ rows WRITE setRows NOTIFY rowsChanged)
public:
	enum Flow {LeftToRight, TopToBottom};
public:
	explicit GridLayoutProperties(QObject *parent = 0);
public:
	int rows() {return m_rows;}
	void setRows(int n);
	Q_SIGNAL void rowsChanged(int n);

	int columns() {return m_columns;}
	void setColumns(int n);
	Q_SIGNAL void columnsChanged(int n);

	Flow flow();
private:
	int m_columns;
	int m_rows;
};

}}

#endif
