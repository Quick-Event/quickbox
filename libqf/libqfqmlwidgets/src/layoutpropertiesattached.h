#ifndef QF_QMLWIDGETS_LAYOUTPROPERTIESATTACHED_H
#define QF_QMLWIDGETS_LAYOUTPROPERTIESATTACHED_H

#include "qmlwidgetsglobal.h"

#include <QObject>
#include <qqml.h>

namespace qf {
namespace qmlwidgets {

class QFQMLWIDGETS_DECL_EXPORT LayoutPropertiesAttached : public QObject
{
	Q_OBJECT
	Q_PROPERTY(int rowSpan READ rowSpan WRITE setRowSpan NOTIFY rowSpanChanged)
public:
	explicit LayoutPropertiesAttached(QObject *parent = 0);
public:
	int rowSpan() {return m_rowSpan;}
	void setRowSpan(int n);
	Q_SIGNAL void rowSpanChanged();
private:
	int m_rowSpan;
};

class QFQMLWIDGETS_DECL_EXPORT LayoutProperties : public QObject
{
    Q_OBJECT
public:
    static LayoutPropertiesAttached *qmlAttachedProperties(QObject *object);
};

}}

QML_DECLARE_TYPEINFO(qf::qmlwidgets::LayoutProperties, QML_HAS_ATTACHED_PROPERTIES)

#endif
