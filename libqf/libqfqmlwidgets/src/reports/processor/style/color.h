#ifndef QF_QMLWIDGETS_REPORTS_STYLE_COLOR_H
#define QF_QMLWIDGETS_REPORTS_STYLE_COLOR_H

#include "../../../qmlwidgetsglobal.h"

#include <qf/core/utils.h>

#include <QObject>

namespace qf {
namespace qmlwidgets {
namespace reports {
namespace style {

class QFQMLWIDGETS_DECL_EXPORT Color : public QObject
{
	Q_OBJECT
	Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
	Q_PROPERTY(QVariant definition READ definition WRITE setDefinition NOTIFY definitionChanged)
public:
	explicit Color(QObject *parent = nullptr);
	~Color() Q_DECL_OVERRIDE;
public:
	QF_PROPERTY_IMPL(QString, n, N, ame)
	QF_PROPERTY_IMPL(QVariant, d, D, efinition)
};

}}}}

#endif
