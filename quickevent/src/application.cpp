#include "application.h"

#include <qf/core/log.h>
#include <qf/qmlwidgets/reports/processor/reportprocessor.h>

#include <QNetworkProxy>
#include <QProcessEnvironment>

Application::Application(int &argc, char **argv)
	: Super(argc, argv)
{
	QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
	QString http_proxy = env.value(QStringLiteral("http_proxy"));
	if(!http_proxy.isEmpty()) {
		if(http_proxy.startsWith(QLatin1String("http://")))
			http_proxy = http_proxy.mid(7);
		QString host = http_proxy.section(':', 0, 0);
		int port = http_proxy.section(':', 1).toInt();
		if(!host.isEmpty() && port > 0) {
			qfInfo() << QString("Setting app http proxy to: %1:%2").arg(host).arg(port);
			QNetworkProxy proxy;
			proxy.setType(QNetworkProxy::HttpProxy);
			proxy.setHostName(host);
			proxy.setPort(port);
			//proxy.setUser("username");
			//proxy.setPassword("password");
			QNetworkProxy::setApplicationProxy(proxy);
		}
	}
	qf::qmlwidgets::reports::ReportProcessor::qmlEngineImportPaths().append(QCoreApplication::applicationDirPath() + "/divers/" + QCoreApplication::applicationName() + "/plugins");
}

Application::~Application()
{
}
