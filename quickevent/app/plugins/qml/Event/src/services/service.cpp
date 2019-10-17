#include "service.h"

#include "../Event/eventplugin.h"

#include <qf/qmlwidgets/framework/dialogwidget.h>
#include <qf/qmlwidgets/framework/mainwindow.h>
#include <qf/qmlwidgets/dialogs/dialog.h>

#include <qf/core/assert.h>

#include <QSettings>

namespace qff = qf::qmlwidgets::framework;
namespace qfd = qf::qmlwidgets::dialogs;

namespace services {

//static const char *KEY_IS_RUNNING = "isRunning";

QList<Service*> Service::m_services;

Service::Service(const QString &name, QObject *parent)
	: QObject(parent)
{
	//qfDebug() << name;
	setObjectName(name);
	setStatus(Status::Stopped);
	connect(eventPlugin(), &Event::EventPlugin::eventOpened, this, &Service::onEventOpen, Qt::QueuedConnection);
}

Service::~Service()
{
	bool is_running = status() == Status::Running;
	ServiceSettings ss = settings();
	ss.setAutoStart(is_running);
	setSettings(ss);
	saveSettings();
}

Event::EventPlugin *Service::eventPlugin()
{
	qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
	auto *plugin = qobject_cast<Event::EventPlugin *>(fwk->plugin("Event"));
	QF_ASSERT(plugin != nullptr, "Bad plugin", return nullptr);
	return plugin;
}

QString Service::settingsGroup() const
{
	QString s = QStringLiteral("services/") + name();
	return s;
}

void Service::onEventOpen()
{
	loadSettings();
	ServiceSettings ss = settings();
	if(ss.isAutoStart()) {
		run();
	}
}

void Service::loadSettings()
{
	qfDebug() << "loading settings for" << settingsGroup();
	m_settings.clear();
	QSettings ss;
	ss.beginGroup(settingsGroup());
	for(const QString &key : ss.childKeys()) {
		m_settings[key] = ss.value(key);
		qfDebug() << "\t" << key << "->" << m_settings.value(key);
	}
}

void Service::saveSettings()
{
	QSettings ss;
	ss.beginGroup(settingsGroup());
	for(const QString &key : m_settings.keys()) {
		ss.setValue(key, m_settings[key]);
	}
}

void Service::setSettings(const QVariantMap &s)
{
	//qfInfo() << __FUNCTION__ << settingsGroup() << s;
	if(!(m_settings == s)) {
		m_settings = s;
		emit settingsChanged();
	}
}

void Service::run()
{
	setStatus(Status::Running);
}

void Service::stop()
{
	setStatus(Status::Stopped);
}

void Service::setRunning(bool on)
{
	if(on && status() == Status::Stopped) {
		run();
	}
	else if(!on && status() == Status::Running) {
		stop();
	}
}

void Service::showDetail(QWidget *parent)
{
	qff::DialogWidget *cw = createDetailWidget();
	if(!cw)
		return;
	qfd::Dialog dlg(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, parent);
	dlg.setCentralWidget(cw);
	dlg.setWindowTitle(name());
	dlg.exec();
}

void Service::addService(Service *service)
{
	if(service)
		m_services << service;
}

Service *Service::serviceAt(int ix)
{
	return m_services.at(ix);
}

Service *Service::serviceByName(const QString &service_name)
{
	for (int i = 0; i < serviceCount(); ++i) {
		Service *svc = serviceAt(i);
		if(svc->name() == service_name) {
			return svc;
		}
	}
	return nullptr;
}

qf::qmlwidgets::framework::DialogWidget *Service::createDetailWidget()
{
	return nullptr;
}

} // namespace services
