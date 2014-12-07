#ifndef QF_QMLWIDGETS_SPINBOX_H
#define QF_QMLWIDGETS_SPINBOX_H

#include "qmlwidgetsglobal.h"
#include "idatawidget.h"

#include <QSpinBox>

namespace qf {
namespace qmlwidgets {

class QFQMLWIDGETS_DECL_EXPORT SpinBox : public QSpinBox
{
	Q_OBJECT
private:
	typedef QSpinBox Super;
public:
	SpinBox(QWidget *parent = nullptr);
	~SpinBox() Q_DECL_OVERRIDE;
};

}}

#endif // QF_QMLWIDGETS_SPINBOX_H
