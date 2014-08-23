#ifndef QF_QMLWIDGETS_DIALOGBUTTONBOX_H
#define QF_QMLWIDGETS_DIALOGBUTTONBOX_H

#include "qmlwidgetsglobal.h"

#include <QDialogButtonBox>

namespace qf {
namespace qmlwidgets {

class QFQMLWIDGETS_DECL_EXPORT DialogButtonBox : public QDialogButtonBox
{
	Q_OBJECT
public:
	explicit DialogButtonBox(QWidget * parent = 0);
	explicit DialogButtonBox(StandardButtons buttons, QWidget *parent = 0);
};

}}

#endif
