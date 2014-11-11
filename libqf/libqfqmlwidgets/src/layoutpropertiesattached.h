#ifndef QF_QMLWIDGETS_LAYOUTPROPERTIESATTACHED_H
#define QF_QMLWIDGETS_LAYOUTPROPERTIESATTACHED_H

#include "qmlwidgetsglobal.h"
#include "layouttypeproperties.h"

#include <qf/core/utils.h>

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
	Q_PROPERTY(qf::qmlwidgets::LayoutTypeProperties::SizePolicy horizontalSizePolicy READ horizontalSizePolicy WRITE setHorizontalSizePolicy NOTIFY horizontalSizePolicyChanged)
	Q_PROPERTY(qf::qmlwidgets::LayoutTypeProperties::SizePolicy verticalSizePolicy READ verticalSizePolicy WRITE setVerticalSizePolicy NOTIFY verticalSizePolicyChanged)
public:
	explicit LayoutPropertiesAttached(QObject *parent = 0);
public:
	QF_PROPERTY_IMPL2(int, r, R, owSpan, 1)
	QF_PROPERTY_IMPL2(int, c, C, olumnSpan, 1)
	QF_PROPERTY_IMPL2(LayoutTypeProperties::SizePolicy, h, H, orizontalSizePolicy, LayoutTypeProperties::Preferred)
	QF_PROPERTY_IMPL2(LayoutTypeProperties::SizePolicy, v, V, erticalSizePolicy, LayoutTypeProperties::Preferred)
public:
	QString buddyText() {return m_formLayoutBuddyText;}
	void setBuddyText(const QString &s);
private:
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
