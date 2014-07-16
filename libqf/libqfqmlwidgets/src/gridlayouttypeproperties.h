#ifndef QF_QMLWIDGETS_GRIDLAYOUTTYPEPROPERTIES_H
#define QF_QMLWIDGETS_GRIDLAYOUTTYPEPROPERTIES_H

#include "qmlwidgetsglobal.h"
#include "layouttypeproperties.h"

#include <QObject>
#include <qqml.h>

namespace qf {
namespace qmlwidgets {

/**
 * @brief The GridLayoutProperties class
 *
 * If the Frame container has grid layout assigned, it is configured according to this properties
 */
class QFQMLWIDGETS_DECL_EXPORT GridLayoutTypeProperties : public LayoutTypeProperties
{
	Q_OBJECT
	Q_PROPERTY(int columns READ columns WRITE setColumns NOTIFY columnsChanged)
	Q_PROPERTY(int rows READ rows WRITE setRows NOTIFY rowsChanged)
private:
	typedef LayoutTypeProperties Super;
public:
	enum Flow {LeftToRight, TopToBottom};
public:
	explicit GridLayoutTypeProperties(QObject *parent = 0);
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
