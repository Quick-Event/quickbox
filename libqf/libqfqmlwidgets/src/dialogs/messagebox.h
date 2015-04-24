#ifndef QF_QMLWIDGETS_DIALOGS_MESSAGEBOX_H
#define QF_QMLWIDGETS_DIALOGS_MESSAGEBOX_H

#include "../qmlwidgetsglobal.h"

#include <QMessageBox>

namespace qf {
namespace core {
class Exception;
}
namespace qmlwidgets {
namespace dialogs {

class QFQMLWIDGETS_DECL_EXPORT MessageBox : public QMessageBox
{
	Q_OBJECT
public:
	explicit MessageBox(QWidget *parent = 0);
public:
	static void showException(QWidget *parent, const QString &what, const QString &where, const QString &stack_trace);
	static void showException(QWidget *parent, const qf::core::Exception &exc);
	static void showError(QWidget *parent, const QString &message);
	static void showInfo(QWidget *parent, const QString &message);
	static bool askYesNo(QWidget *parent, const QString & msg, bool default_ret = true);
};

}}}

#endif // QF_QMLWIDGETS_DIALOGS_MESSAGEBOX_H
