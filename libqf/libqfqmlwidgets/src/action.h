#ifndef QF_QMLWIDGETS_ACTION_H
#define QF_QMLWIDGETS_ACTION_H

#include "qmlwidgetsglobal.h"

#include <QAction>

namespace qf {
namespace qmlwidgets {

class QFQMLWIDGETS_DECL_EXPORT Action : public QAction
{
	Q_OBJECT
	Q_PROPERTY(QString oid READ oid WRITE setOid FINAL)
	Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
	Q_PROPERTY(QString shortcut READ shortcut WRITE setShortcut NOTIFY shortcutChanged)
private:
	typedef QAction Super;
public:
	typedef QMap<QString, Action*> ActionMap;
public:
	explicit Action(const QIcon &icon, const QString &text, QObject *parent = 0);
	explicit Action(QObject *parent = 0) : Action(QIcon(), QString(), parent) {}
	explicit Action(const QString &text, QObject *parent = 0) : Action(QIcon(), text, parent) {}
	explicit Action(const QString &oid, const QString &text, QObject *parent = 0) : Action(QIcon(), text, parent) {setOid(oid);}

	QString oid() {return objectName();}
	void setOid(const QString &id) {setObjectName(id);}

	QString text() const {return Super::text();}
	void setText(const QString &new_text);
	Q_SIGNAL void textChanged(const QString &new_text);

	QString shortcut() const;
	void setShortcut(const QString &new_text);
	Q_SIGNAL void shortcutChanged(const QString &new_text);
	using Super::setShortcut;

	Action *addActionInto(const QString &id, const QString &text);
	Q_INVOKABLE void addActionInto(QAction *action);
	Q_INVOKABLE void addActionBefore(QAction *action);
	Q_INVOKABLE void addActionAfter(QAction *new_act);
	Q_INVOKABLE qf::qmlwidgets::Action *addMenuInto(const QString &id, const QString &text);
	Q_INVOKABLE qf::qmlwidgets::Action *addMenuAfter(const QString &id, const QString &text);
	Q_INVOKABLE qf::qmlwidgets::Action *addMenuBefore(const QString &id, const QString &text);
	Q_INVOKABLE qf::qmlwidgets::Action *addSeparatorInto(const QString &id = QString());
	Q_INVOKABLE qf::qmlwidgets::Action *addSeparatorBefore(const QString &id = QString());
private:
	QWidget* parentMenu();
};

}
}

#endif // ACTION_H
