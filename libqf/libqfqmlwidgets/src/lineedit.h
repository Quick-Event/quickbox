#ifndef QF_QMLWIDGETS_LINEEDIT_H
#define QF_QMLWIDGETS_LINEEDIT_H

#include "qmlwidgetsglobal.h"

#include <QLineEdit>

namespace qf {
namespace qmlwidgets {

class QFQMLWIDGETS_DECL_EXPORT LineEdit : public QLineEdit
{
	Q_OBJECT
public:
	explicit LineEdit(QWidget *parent = 0);

signals:

public slots:

};

}}

#endif // QF_QMLWIDGETS_LINEEDIT_H
