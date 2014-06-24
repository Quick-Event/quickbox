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
	Q_PROPERTY(int columnSpan READ rowSpan WRITE setColumnSpan NOTIFY columnSpanChanged)
	Q_PROPERTY(QString buddyText READ buddyText WRITE setBuddyText)
public:
	explicit LayoutPropertiesAttached(QObject *parent = 0);
public:
	int rowSpan() {return m_rowSpan;}
	void setRowSpan(int n);
	Q_SIGNAL void rowSpanChanged();
	int columnSpan() {return m_columnSpan;}
	void setColumnSpan(int n);
	Q_SIGNAL void columnSpanChanged();

	QString buddyText() {return m_formLayoutBuddyText;}
	void setBuddyText(const QString &s);
private:
	int m_rowSpan;
	int m_columnSpan;
	QString m_formLayoutBuddyText;
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
