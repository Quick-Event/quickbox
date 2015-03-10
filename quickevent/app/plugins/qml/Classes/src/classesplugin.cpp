#include "classesplugin.h"
#include "thispartwidget.h"

#include <qf/qmlwidgets/framework/mainwindow.h>
//#include <qf/qmlwidgets/dialogs/dialog.h>
//#include <qf/qmlwidgets/dialogs/messagebox.h>
//#include <qf/qmlwidgets/dialogs/filedialog.h>
#include <qf/qmlwidgets/action.h>
//#include <qf/qmlwidgets/menubar.h>
//#include <qf/qmlwidgets/statusbar.h>
//#include <qf/qmlwidgets/toolbar.h>

//#include <qf/core/log.h>
//#include <qf/core/sql/query.h>
//#include <qf/core/sql/querybuilder.h>
//#include <qf/core/sql/connection.h>
//#include <qf/core/sql/transaction.h>

//#include <QInputDialog>
//#include <QSqlDatabase>
//#include <QSqlError>
//#include <QComboBox>
//#include <QLabel>
//#include <QMetaObject>

namespace qfw = qf::qmlwidgets;
namespace qff = qf::qmlwidgets::framework;
//namespace qfd = qf::qmlwidgets::dialogs;
//namespace qfs = qf::core::sql;

ClassesPlugin::ClassesPlugin(QObject *parent)
	: Super(parent)
{
	connect(this, &ClassesPlugin::installed, this, &ClassesPlugin::onInstalled, Qt::QueuedConnection);
}

void ClassesPlugin::onInstalled()
{
	qff::MainWindow *fwk = qff::MainWindow::frameWork();
	m_partWidget = new ThisPartWidget();
	fwk->addPartWidget(m_partWidget, manifest()->featureId());

	emit nativeInstalled();
}

