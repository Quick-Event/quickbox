#ifndef QF_QMLWIDGETS_BOXLAYOUTTYPEPROPERTIES_H
#define QF_QMLWIDGETS_BOXLAYOUTTYPEPROPERTIES_H

#include "qmlwidgetsglobal.h"
#include "layouttypeproperties.h"

#include <QObject>
#include <qqml.h>

namespace qf {
namespace qmlwidgets {

/**
 * @brief The BoxLayoutTypeProperties class
 *
 * If the Frame container has horizontal or vertical layout assigned, it is configured according to this properties
 */
class QFQMLWIDGETS_DECL_EXPORT BoxLayoutTypeProperties : public LayoutTypeProperties
{
	Q_OBJECT
	Q_PROPERTY(int spacing READ spacing WRITE setSpacing NOTIFY spacingChanged)
private:
	typedef LayoutTypeProperties Super;
public:
	explicit BoxLayoutTypeProperties(QObject *parent = 0);
public:
	int spacing() {return m_spacing;}
	void setSpacing(int n);
	Q_SIGNAL void spacingChanged(int n);
private:
	int m_spacing;
};

}}

#endif
