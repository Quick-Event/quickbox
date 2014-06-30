#ifndef QF_QMLWIDGETS_DIALOGS_INPUTDIALOG_H
#define QF_QMLWIDGETS_DIALOGS_INPUTDIALOG_H

#include "../qmlwidgetsglobal.h"

#include <QInputDialog>

namespace qf {
namespace qmlwidgets {
namespace dialogs {

class QFQMLWIDGETS_DECL_EXPORT InputDialog : public QInputDialog
{
	Q_OBJECT
	Q_PROPERTY(InputMode inputMode READ inputMode WRITE setInputMode)
    Q_PROPERTY(QString labelText READ labelText WRITE setLabelText)
    Q_PROPERTY(InputDialogOptions options READ options WRITE setOptions)
    Q_PROPERTY(QString textValue READ textValue WRITE setTextValue)
    Q_PROPERTY(int intValue READ intValue WRITE setIntValue)
    Q_PROPERTY(int doubleValue READ doubleValue WRITE setDoubleValue)
    Q_PROPERTY(QLineEdit::EchoMode textEchoMode READ textEchoMode WRITE setTextEchoMode)
    Q_PROPERTY(bool comboBoxEditable READ isComboBoxEditable WRITE setComboBoxEditable)
    Q_PROPERTY(QStringList comboBoxItems READ comboBoxItems WRITE setComboBoxItems)
    Q_PROPERTY(int intMinimum READ intMinimum WRITE setIntMinimum)
    Q_PROPERTY(int intMaximum READ intMaximum WRITE setIntMaximum)
    Q_PROPERTY(int intStep READ intStep WRITE setIntStep)
    Q_PROPERTY(double doubleMinimum READ doubleMinimum WRITE setDoubleMinimum)
    Q_PROPERTY(double doubleMaximum READ doubleMaximum WRITE setDoubleMaximum)
    Q_PROPERTY(int doubleDecimals READ doubleDecimals WRITE setDoubleDecimals)
    Q_PROPERTY(QString okButtonText READ okButtonText WRITE setOkButtonText)
    Q_PROPERTY(QString cancelButtonText READ cancelButtonText WRITE setCancelButtonText)
public:
	explicit InputDialog(QWidget *parent = 0, Qt::WindowFlags flags = 0);
};

}}}

#endif // QF_QMLWIDGETS_DIALOGS_INPUTDIALOG_H
