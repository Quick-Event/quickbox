#ifndef QF_QMLWIDGETS_REPORTS_STYLE_COLOR_H
#define QF_QMLWIDGETS_REPORTS_STYLE_COLOR_H

#include "styleobject.h"

#include "../../../qmlwidgetsglobal.h"

#include <qf/core/utils.h>

#include <QObject>
#include <QColor>

namespace qf {
namespace qmlwidgets {
namespace reports {
namespace style {

class QFQMLWIDGETS_DECL_EXPORT Color : public StyleObject
{
	Q_OBJECT
    Q_PROPERTY(QVariant def READ definition WRITE setDefinition NOTIFY definitionChanged)
public:
	explicit Color(QObject *parent = nullptr);
	~Color() Q_DECL_OVERRIDE;

	QF_PROPERTY_IMPL(QVariant, d, D, efinition)
public:
	QColor color();
private:
    //Q_SLOT void setDirty();
private:
	QColor m_color;
};

}}}}

#endif
