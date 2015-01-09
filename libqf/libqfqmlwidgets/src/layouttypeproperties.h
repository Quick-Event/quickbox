#ifndef QF_QMLWIDGETS_LAYOUTTYPEPROPERTIES_H
#define QF_QMLWIDGETS_LAYOUTTYPEPROPERTIES_H

#include "qmlwidgetsglobal.h"

#include <QObject>
#include <QSizePolicy>

namespace qf {
namespace qmlwidgets {

/**
 * @brief The LayoutTypeProperties class
 *
 * If the Frame container has grid layout assigned, it is configured according to this properties
 */
class QFQMLWIDGETS_DECL_EXPORT LayoutTypeProperties : public QObject
{
	Q_OBJECT
	Q_ENUMS(SizePolicy)
	Q_PROPERTY(int spacing READ spacing WRITE setSpacing NOTIFY spacingChanged)
	Q_PROPERTY(int columns READ columns WRITE setColumns NOTIFY columnsChanged)
	Q_PROPERTY(int rows READ rows WRITE setRows NOTIFY rowsChanged)
private:
	typedef QObject Super;
public:
	enum Flow {LeftToRight, TopToBottom};
	enum SizePolicy {
		Fixed = QSizePolicy::Fixed,
		Minimum = QSizePolicy::Minimum,
		Maximum = QSizePolicy::Maximum,
		Preferred = QSizePolicy::Preferred,
		Expanding = QSizePolicy::Expanding,
		MinimumExpanding = QSizePolicy::MinimumExpanding,
		Ignored = QSizePolicy::Ignored
	};
public:
	explicit LayoutTypeProperties(QObject *parent = 0);
public:
	int rows() {return m_rows;}
	void setRows(int n);
	Q_SIGNAL void rowsChanged(int n);

	int columns() {return m_columns;}
	void setColumns(int n);
	Q_SIGNAL void columnsChanged(int n);

	int spacing() {return m_spacing;}
	void setSpacing(int n);
	Q_SIGNAL void spacingChanged(int n);

	Flow flow();
private:
	int m_columns;
	int m_rows;
	int m_spacing;
};

}}

#endif
