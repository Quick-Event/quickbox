#ifndef QF_QMLWIDGETS_STATUSBAR_H
#define QF_QMLWIDGETS_STATUSBAR_H

#include "qmlwidgetsglobal.h"

#include <QStatusBar>

class QWidget;

namespace qf {
namespace qmlwidgets {

class QFQMLWIDGETS_DECL_EXPORT StatusBar : public QStatusBar
{
	Q_OBJECT
private:
	typedef QStatusBar Super;
public:
	explicit StatusBar(QWidget *parent = 0);
	~StatusBar() Q_DECL_OVERRIDE;
public:
	Q_SLOT void showMessage(const QString &message, int timeout = 0);
	Q_INVOKABLE void addPermanentWidget(QWidget *widget, int stretch = 0);
	Q_INVOKABLE void addWidget(QWidget *widget, int stretch = 0);
};

}}

#endif
