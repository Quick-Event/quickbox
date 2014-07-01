#ifndef QF_QMLWIDGETS_DIALOGS_MESSAGEBOX_H
#define QF_QMLWIDGETS_DIALOGS_MESSAGEBOX_H

#include "../qmlwidgetsglobal.h"

#include <QMessageBox>

namespace qf {
namespace qmlwidgets {
namespace dialogs {

class QFQMLWIDGETS_DECL_EXPORT MessageBox : public QMessageBox
{
	Q_OBJECT
public:
	explicit MessageBox(QWidget *parent = 0);

signals:

public slots:

};

}}}

#endif // QF_QMLWIDGETS_DIALOGS_MESSAGEBOX_H
