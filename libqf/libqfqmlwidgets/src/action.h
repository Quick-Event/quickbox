#ifndef ACTION_H
#define ACTION_H

#include "qmlwidgetsglobal.h"

#include <QAction>

namespace qf {
namespace qmlwidgets {

class QFQMLWIDGETS_DECL_EXPORT Action : public QAction
{
	Q_OBJECT
	Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
private:
	typedef QAction Super;
public:
	explicit Action(QObject *parent = 0);

	void setText(const QString &new_text);
	QString text() const {return Super::text();}
signals:
	void textChanged(const QString &new_text);
};

}
}

#endif // ACTION_H
