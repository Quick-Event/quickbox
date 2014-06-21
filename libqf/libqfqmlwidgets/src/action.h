#ifndef QF_QMLWIDGETS_ACTION_H
#define QF_QMLWIDGETS_ACTION_H

#include "qmlwidgetsglobal.h"

#include <QAction>

namespace qf {
namespace qmlwidgets {

class QFQMLWIDGETS_DECL_EXPORT Action : public QAction
{
	Q_OBJECT
	Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
	Q_PROPERTY(QString shortcut READ shortcut WRITE setShortcut NOTIFY shortcutChanged)
private:
	typedef QAction Super;
public:
	explicit Action(QObject *parent = 0);

	QString text() const {return Super::text();}
	void setText(const QString &new_text);
	Q_SIGNAL void textChanged(const QString &new_text);

	QString shortcut() const;
	void setShortcut(const QString &new_text);
	Q_SIGNAL void shortcutChanged(const QString &new_text);
signals:
};

}
}

#endif // ACTION_H
