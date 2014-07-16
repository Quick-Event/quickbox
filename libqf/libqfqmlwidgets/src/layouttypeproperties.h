#ifndef QF_QMLWIDGETS_LAYOUTTYPEPROPERTIES_H
#define QF_QMLWIDGETS_LAYOUTTYPEPROPERTIES_H

#include "qmlwidgetsglobal.h"

#include <QObject>

namespace qf {
namespace qmlwidgets {

/**
 * @brief The GridLayoutProperties class
 *
 * If the Frame container has grid layout assigned, it is configured according to this properties
 */
class QFQMLWIDGETS_DECL_EXPORT LayoutTypeProperties : public QObject
{
	Q_OBJECT
private:
	typedef QObject Super;
public:
	explicit LayoutTypeProperties(QObject *parent = 0);
};

}}

#endif
