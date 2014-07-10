#ifndef QF_QMLWIDGETS_TABLEVIEW_H
#define QF_QMLWIDGETS_TABLEVIEW_H

#include "qmlwidgetsglobal.h"

#include <QTableView>

namespace qf {
namespace qmlwidgets {

class QFQMLWIDGETS_DECL_EXPORT TableView : public QTableView
{
	Q_OBJECT
public:
	explicit TableView(QWidget *parent = 0);
};

}}

#endif // QF_QMLWIDGETS_TABLEVIEW_H
