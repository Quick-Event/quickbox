#ifndef QF_QMLWIDGETS_DIALOG_H
#define QF_QMLWIDGETS_DIALOG_H

#include "qmlwidgetsglobal.h"
#include "framework/ipersistentsettings.h"

#include <QDialog>
#include <QQmlListProperty>

namespace qf {
namespace qmlwidgets {

class Frame;

class QFQMLWIDGETS_DECL_EXPORT Dialog : public QDialog, public framework::IPersistentSettings
{
	Q_OBJECT
	Q_PROPERTY(QQmlListProperty<QWidget> widgets READ widgets)
	Q_CLASSINFO("DefaultProperty", "widgets")
public:
	explicit Dialog(QWidget *parent = 0);
	~Dialog() Q_DECL_OVERRIDE;

	int exec() Q_DECL_OVERRIDE;
protected:
	QQmlListProperty<QWidget> widgets();
private:
	Q_SLOT void loadPersistentSettings();
	Q_SLOT void savePersistentSettings();
protected:
	Frame *m_centralFrame;
};

}}

#endif // QF_QMLWIDGETS_DIALOG_H
