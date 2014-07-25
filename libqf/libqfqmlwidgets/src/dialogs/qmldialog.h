#ifndef QF_QMLWIDGETS_DIALOGS_QMLDIALOG_H
#define QF_QMLWIDGETS_DIALOGS_QMLDIALOG_H

#include "../qmlwidgetsglobal.h"
#include "dialog.h"
#include "../framework/ipersistentsettings.h"

#include <QQmlListProperty>
#include <QQmlParserStatus>

namespace qf {
namespace qmlwidgets {

class Frame;
class DialogButtonBox;

namespace dialogs {

class QFQMLWIDGETS_DECL_EXPORT QmlDialog : public Dialog, public QQmlParserStatus
{
	Q_OBJECT
	Q_INTERFACES(QQmlParserStatus)
	Q_PROPERTY(QQmlListProperty<QWidget> widgets READ widgets)
	Q_CLASSINFO("DefaultProperty", "widgets")
	Q_PROPERTY(qf::qmlwidgets::DialogButtonBox* buttonBox READ buttonBox WRITE setButtonBox NOTIFY buttonBoxChanged)
private:
	typedef Dialog Super;
public:
	explicit QmlDialog(QWidget *parent = 0);
	~QmlDialog() Q_DECL_OVERRIDE;
public:
	DialogButtonBox* buttonBox() {return m_dialogButtonBox;}
	void setButtonBox(DialogButtonBox *dbb);
	Q_SIGNAL void buttonBoxChanged();
protected:
	QQmlListProperty<QWidget> widgets();
private:
	void classBegin() Q_DECL_OVERRIDE;
	void componentComplete() Q_DECL_OVERRIDE;
protected:
	Frame *m_centralFrame;
	DialogButtonBox *m_dialogButtonBox;
};

}}}

#endif // QF_QMLWIDGETS_DIALOGS_QMLDIALOG_H
