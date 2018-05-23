#include "relaysplugin.h"
#include "../thispartwidget.h"
#include "relaydocument.h"
#include "../relaywidget.h"

#include <Event/eventplugin.h>

#include <qf/qmlwidgets/framework/mainwindow.h>
#include <qf/qmlwidgets/framework/dockwidget.h>
#include <qf/qmlwidgets/dialogs/dialog.h>
#include <qf/qmlwidgets/action.h>
#include <qf/qmlwidgets/menubar.h>

#include <qf/core/model/sqltablemodel.h>
#include <qf/core/log.h>
#include <qf/core/assert.h>

#include <QQmlEngine>

namespace qfw = qf::qmlwidgets;
namespace qff = qf::qmlwidgets::framework;
namespace qfd = qf::qmlwidgets::dialogs;
namespace qfm = qf::core::model;
namespace qfs = qf::core::sql;

namespace Relays {

static Event::EventPlugin* eventPlugin()
{
	qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
	auto *plugin = qobject_cast<Event::EventPlugin*>(fwk->plugin("Event"));
	QF_ASSERT_EX(plugin != nullptr, "Bad Event plugin!");
	return plugin;
}

RelaysPlugin::RelaysPlugin(QObject *parent)
	: Super(parent)
{
	connect(this, &RelaysPlugin::installed, this, &RelaysPlugin::onInstalled, Qt::QueuedConnection);
}

RelaysPlugin::~RelaysPlugin()
{
	//if(m_registrationsDockWidget)
	//	m_registrationsDockWidget->savePersistentSettingsRecursively();
}

QObject *RelaysPlugin::createRelayDocument(QObject *parent)
{
	 RelayDocument *ret = new  RelayDocument(parent);
	if(!parent) {
		qfWarning() << "Parent is NULL, created class will have QQmlEngine::JavaScriptOwnership.";
		qmlEngine()->setObjectOwnership(ret, QQmlEngine::JavaScriptOwnership);
	}
	return ret;
}

int RelaysPlugin::editRelay(int id, int mode)
{
	qfLogFuncFrame() << "id:" << id;
	auto *w = new  RelayWidget();
	w->setWindowTitle(tr("Edit  Relay"));
	qfd::Dialog dlg(QDialogButtonBox::Save | QDialogButtonBox::Cancel, m_partWidget);
	dlg.setDefaultButton(QDialogButtonBox::Save);
	dlg.setCentralWidget(w);
	w->load(id, (qfm::DataDocument::RecordEditMode)mode);
	return dlg.exec();
}

void RelaysPlugin::onInstalled()
{
	qff::MainWindow *fwk = qff::MainWindow::frameWork();
	m_partWidget = new ThisPartWidget();
	fwk->addPartWidget(m_partWidget, manifest()->featureId());

	connect(eventPlugin(), &Event::EventPlugin::dbEventNotify, this, &RelaysPlugin::onDbEventNotify);

	emit nativeInstalled();
}

void RelaysPlugin::onDbEventNotify(const QString &domain, int connection_id, const QVariant &data)
{
	Q_UNUSED(connection_id)
	qfLogFuncFrame() << "domain:" << domain << "payload:" << data;
	emit dbEventNotify(domain, connection_id, data);
}

}
