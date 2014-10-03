#ifndef QF_QMLWIDGETS_REPORTS_STYLE_SHEET_H
#define QF_QMLWIDGETS_REPORTS_STYLE_SHEET_H

#include "../../../qmlwidgetsglobal.h"

#include <qf/core/utils.h>

#include <QObject>
#include <QString>

namespace qf {
namespace qmlwidgets {
namespace reports {
namespace style {

class QFQMLWIDGETS_DECL_EXPORT Sheet : public QObject
{
	Q_OBJECT
	Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
public:
	explicit Sheet(QObject *parent = nullptr);
public:
	QF_PROPERTY_IMPL(QString, n, N, ame)
};

}}}}

#endif // QF_QMLWIDGETS_REPORTS_STYLE_SHEET_H
