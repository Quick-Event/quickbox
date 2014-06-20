#ifndef QF_QMLWIDGETS_MENUBAR_H
#define QF_QMLWIDGETS_MENUBAR_H

#include "qmlwidgetsglobal.h"

#include <QMenuBar>

namespace qf {
namespace qmlwidgets {

class Menu;

class QFQMLWIDGETS_DECL_EXPORT MenuBar : public QMenuBar
{
	Q_OBJECT
public:
	explicit MenuBar(QWidget *parent = 0);

signals:

public slots:
	QObject* itemForPath(const QString &path, bool create_if_not_exists = true);
};

}}

#endif // QF_QMLWIDGETS_MENUBAR_H
