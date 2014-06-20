#ifndef QF_QMLWIDGETS_DIALOG_H
#define QF_QMLWIDGETS_DIALOG_H

#include "qmlwidgetsglobal.h"

#include <QDialog>
#include <QQmlListProperty>

namespace qf {
namespace qmlwidgets {

class Frame;

class QFQMLWIDGETS_DECL_EXPORT Dialog : public QDialog
{
	Q_OBJECT
	Q_PROPERTY(QQmlListProperty<QWidget> widgets READ widgets)
	Q_CLASSINFO("DefaultProperty", "widgets")
public:
	explicit Dialog(QWidget *parent = 0);

signals:

public slots:
protected:
	QQmlListProperty<QWidget> widgets();
protected:
	Frame *m_centralFrame;
};

}}

#endif // QF_QMLWIDGETS_DIALOG_H
