#ifndef QF_QMLWIDGETS_GRIDLAYOUTPROPERTIESATTACHED_H
#define QF_QMLWIDGETS_GRIDLAYOUTPROPERTIESATTACHED_H

#include "qmlwidgetsglobal.h"

#include <QObject>
#include <qqml.h>

namespace qf {
namespace qmlwidgets {

class QFQMLWIDGETS_DECL_EXPORT GridLayoutPropertiesAttached : public QObject
{
	Q_OBJECT
	Q_PROPERTY(int columns READ columns WRITE setColumns NOTIFY columnsChanged)
	Q_PROPERTY(int rows READ rows WRITE setRows NOTIFY rowsChanged)
public:
	explicit GridLayoutPropertiesAttached(QObject *parent = 0);
private:
	int rows() {return m_rows;}
	void setRows(int n);
	Q_SIGNAL void rowsChanged(int n);

	int columns() {return m_columns;}
	void setColumns(int n);
	Q_SIGNAL void columnsChanged(int n);
private:
	int m_columns;
	int m_rows;
};

class QFQMLWIDGETS_DECL_EXPORT GridLayoutProperties : public QObject
{
    Q_OBJECT
public:
    static GridLayoutPropertiesAttached *qmlAttachedProperties(QObject *object);
};

}}

QML_DECLARE_TYPEINFO(qf::qmlwidgets::GridLayoutProperties, QML_HAS_ATTACHED_PROPERTIES)

#endif
