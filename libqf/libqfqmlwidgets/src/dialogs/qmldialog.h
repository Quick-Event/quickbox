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

namespace dialogs {

class QFQMLWIDGETS_DECL_EXPORT QmlDialog : public Dialog, public QQmlParserStatus
{
	Q_OBJECT
	Q_INTERFACES(QQmlParserStatus)
	Q_PROPERTY(QQmlListProperty<QWidget> widgets READ widgets)
	Q_CLASSINFO("DefaultProperty", "widgets")
private:
	typedef Dialog Super;
public:
	explicit QmlDialog(QWidget *parent = 0);
	~QmlDialog() Q_DECL_OVERRIDE;
protected:
	QQmlListProperty<QWidget> widgets();
private:
	void classBegin() Q_DECL_OVERRIDE;
	void componentComplete() Q_DECL_OVERRIDE;
protected:
	Frame *m_centralFrame;
};

}}}

#endif // QF_QMLWIDGETS_DIALOGS_QMLDIALOG_H
