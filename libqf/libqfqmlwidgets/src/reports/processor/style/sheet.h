#ifndef QF_QMLWIDGETS_REPORTS_STYLE_SHEET_H
#define QF_QMLWIDGETS_REPORTS_STYLE_SHEET_H

#include "../../../qmlwidgetsglobal.h"

#include <QObject>

namespace qf {
namespace qmlwidgets {
namespace reports {
namespace style {

class QFQMLWIDGETS_DECL_EXPORT Sheet : public QObject
{
	Q_OBJECT
public:
	explicit Sheet(QObject *parent = nullptr);

};

}}}}

#endif // QF_QMLWIDGETS_REPORTS_STYLE_SHEET_H
