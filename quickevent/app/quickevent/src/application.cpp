#include "application.h"
#include "appclioptions.h"

#include <qf/qmlwidgets/reports/processor/reportprocessor.h>
#include <qf/qmlwidgets/style.h>

#include <qf/core/log.h>

#include <QNetworkProxy>
#include <QProcessEnvironment>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

Application::Application(int &argc, char **argv, AppCliOptions *cli_opts)
	: Super(argc, argv)
	, m_cliOptions(cli_opts)
{
	if(cli_opts->appFontScale() != 1.) {
		QFont app_font = font();
		app_font.setPointSizeF(app_font.pointSizeF() * cli_opts->appFontScale());
		setFont(app_font);
	}

	auto *style = qf::qmlwidgets::Style::instance();
	style->setIconPath(":/qf/qmlwidgets/images/flat");

	loadStyleSheet();

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
#ifdef Q_OS_UNIX
	QString plugin_path = QCoreApplication::applicationDirPath() + "/../lib/qml/" + QCoreApplication::applicationName().toLower();
#else
	QString plugin_path = QCoreApplication::applicationDirPath() + "/qml/" + QCoreApplication::applicationName().toLower();
#ifdef Q_OS_WIN
	qfInfo() << "Adding DLL search path:" << plugin_path;
	SetDllDirectory(reinterpret_cast<LPCWSTR>(plugin_path.utf16()));
#endif
#endif
	qf::qmlwidgets::reports::ReportProcessor::qmlEngineImportPaths().append(plugin_path);
}

Application::~Application()
{
}

Application *Application::instance(bool must_exist)
{
	Application *ret = qobject_cast<Application*>(Super::instance());
	if(!ret && must_exist) {
		qfFatal("Application instance MUST exist.");
	}
	return ret;
}

int Application::dbVersion()
{
	return 10900;
}

QString Application::versionString() const
{
	return QCoreApplication::applicationVersion();
}

QString Application::dbVersionString() const
{
	int dbv = dbVersion();
	int rev = dbv % 100;
	dbv /= 100;
	int min = dbv % 100;
	int maj = dbv / 100;

	return QString("%1.%2.%3").arg(maj).arg(min).arg(rev);
}
