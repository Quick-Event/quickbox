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
	qf::qmlwidgets::Menu* ensureMenuOnPath(const QString &path);
};

}}

#endif // QF_QMLWIDGETS_MENUBAR_H
