#include "service.h"

#include <qf/qmlwidgets/framework/dialogwidget.h>
#include <qf/qmlwidgets/framework/mainwindow.h>
#include <qf/qmlwidgets/dialogs/dialog.h>

namespace qff = qf::qmlwidgets::framework;
namespace qfd = qf::qmlwidgets::dialogs;

namespace services {

QList<Service*> Service::m_services;

Service::Service(QObject *parent)
	: QObject(parent)
{
	setStatus(Status::Stopped);
}

void Service::loadConfig()
{

}

void Service::run()
{
	//setStatus(Status::Starting);
	setStatus(Status::Running);
}

void Service::stop()
{
	setStatus(Status::Stopped);
}

void Service::setRunning(bool on)
{
	if(on && status() == Status::Stopped) {
		loadConfig();
		run();
	}
	else if(!on && status() == Status::Running) {
		stop();
	}
}

void Service::showDetail()
{
	qff::DialogWidget *cw = createDetailWidget();
	if(!cw)
		return;
	qff::MainWindow *fwk = qff::MainWindow::frameWork();
	qfd::Dialog dlg(fwk);
	dlg.setCentralWidget(cw);
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

qf::qmlwidgets::framework::DialogWidget *Service::createDetailWidget()
{
	return nullptr;
}

} // namespace services
