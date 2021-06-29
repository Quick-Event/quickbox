#include "application.h"

#include <qf/core/log.h>
#include <qf/core/assert.h>
#include <qf/core/utils/fileutils.h>

#include <QQmlEngine>
#include <QQmlContext>
#include <QFile>
#include <QJsonParseError>

namespace qfu = qf::core::utils;
using namespace qf::qmlwidgets::framework;

Application::Application(int &argc, char **argv) :
	Super(argc, argv), m_qmlEngine(nullptr)
{
	m_pluginDataDir = ":/quickevent";
/*
	#ifdef Q_OS_UNIX
		m_pluginDataDir = QCoreApplication::applicationDirPath() + "/../lib/qml/quickevent";
	#else
		m_pluginDataDir = QCoreApplication::applicationDirPath() + "/qml/quickevent";
	#endif
*/
		QString path;
	#ifdef Q_OS_UNIX
		path = QCoreApplication::applicationDirPath() + "/../lib/qml";
	#else
		path = QCoreApplication::applicationDirPath() + "/qml";
	#endif
	m_qmlLibraryImportPaths << path;
/*
	QString path;
#ifdef Q_OS_UNIX
		path = QCoreApplication::applicationDirPath() + "/../lib/qml/" + QCoreApplication::applicationName().toLower();
#else
		path = QCoreApplication::applicationDirPath() + "/qml/" + QCoreApplication::applicationName().toLower();
#endif
	m_qmlPluginImportPaths << path;
*/
	m_qmlPluginImportPaths << ":/";

	//loadStyleSheet();
}

Application::~Application()
{
	releaseQmlEngine();
}

Application *Application::instance(bool must_exist)
{
	Application *ret = qobject_cast<Application*>(Super::instance());
	if(!ret && must_exist) {
		qfFatal("qf::qmlwidgets::framework::Application instance MUST exist.");
	}
	return ret;
}

QQmlEngine *Application::qmlEngine()
{
	if(!m_qmlEngine) {
		setQmlEngine(createQmlEngine());
	}
	Q_ASSERT(m_qmlEngine != nullptr);
	return m_qmlEngine;
}

void Application::setQmlEngine(QQmlEngine *eng)
{
	releaseQmlEngine();
	m_qmlEngine = eng;
	m_qmlEngine->rootContext()->setContextProperty("Application", this);
	setupQmlImportPaths();
	connect(m_qmlEngine, &QQmlEngine::warnings, this, &Application::onQmlError);
	connect(m_qmlEngine, &QQmlEngine::quit, this, &Application::quit);
}

const QList<QQmlError> &Application::qmlErrorList()
{
	return m_qmlErrorList;
}

void Application::clearQmlErrorList()
{
	m_qmlErrorList.clear();
}

MainWindow *Application::frameWork()
{
	QF_ASSERT_EX(m_frameWork != nullptr, "FrameWork is not set.");
	return m_frameWork;
}

QString Application::applicationDirPath()
{
	return QCoreApplication::applicationDirPath();
}

QString Application::applicationName()
{
	return QCoreApplication::applicationName();
}

QStringList Application::arguments()
{
	return QCoreApplication::arguments();
}

QQmlEngine *Application::createQmlEngine()
{
	QQmlEngine *ret = new QQmlEngine(this);
	return ret;
}

void Application::setupQmlImportPaths()
{
	QF_ASSERT(m_qmlEngine != nullptr, "", return);

	Q_FOREACH(auto path, m_qmlLibraryImportPaths) {
		qfInfo() << "Adding QML library import path:" << path;
		m_qmlEngine->addImportPath(path);
	}
	Q_FOREACH(auto path, m_qmlPluginImportPaths) {
		qfInfo() << "Adding QML plugin import path:" << path;
		m_qmlEngine->addImportPath(path);
	}
}

void Application::onQmlError(const QList<QQmlError> &qmlerror_list)
{
	Q_FOREACH(auto err, qmlerror_list) {
		qfError() << "QML ERROR:" << err.toString();
	}
	m_qmlErrorList << qmlerror_list;
}

QJsonDocument Application::profile()
{
	if(!m_profileLoaded) {
		m_profileLoaded = true;
		const QStringList args = arguments();
		for(int i=0; i<args.count(); i++) {
			QString arg = args[i];
			if(arg == QLatin1String("--profile")) {
				QString profile_path = args.value(++i);
				if(!profile_path.isEmpty()) {
					if(!profile_path.contains('.'))
						profile_path += ".profile";
#ifdef Q_OS_UNIX
					if(!profile_path.contains('/'))
						profile_path = QCoreApplication::applicationDirPath() + '/' + profile_path;
#endif
#ifdef Q_OS_WIN
					if(!profile_path.contains('\\'))
						profile_path = QCoreApplication::applicationDirPath() + '/' + profile_path;
#endif
					QFile f(profile_path);
					if(!f.open(QIODevice::ReadOnly)) {
						qfError() << "Cannot open profile file" << f.fileName() << "for reading.";
					}
					else {
						QByteArray ba = f.readAll();
						QJsonParseError err;
						m_profile = QJsonDocument::fromJson(ba, &err);
						if(err.error != QJsonParseError::NoError) {
							qfError() << "Error loading profile file" << f.fileName() << err.errorString();
						}
					}
				}
			}
		}
	}
	return m_profile;
}
/*
bool Application::loadStyleSheet(const QUrl &url)
{
	QString css_file_name = url.toLocalFile();
	QFile f(css_file_name);
	if(f.open(QFile::ReadOnly)) {
		QByteArray ba = f.readAll();
		QString ss = QString::fromUtf8(ba);
		setStyleSheet(ss);
		return true;
	}
	//qfWarning() << "Cannot open style sheet:" << css_file_name;
	return false;
}
*/
void Application::loadStyleSheet(const QString &file)
{
	QString css_file_name = file;
	if(css_file_name.isEmpty()) {
		QString app_name = Application::applicationName().toLower();
		css_file_name = qfu::FileUtils::joinPath(Application::applicationDirPath(), "/" + app_name + "-data/style/default.css");
		if(!QFile::exists(css_file_name))
			css_file_name = ":/" + app_name + "/style/default.css";
	}
	qfInfo() << "Opening style sheet:" << css_file_name;
	QFile f(css_file_name);
	if(f.open(QFile::ReadOnly)) {
		QByteArray ba = f.readAll();
		QString ss = QString::fromUtf8(ba);
		setStyleSheet(ss);
	}
	else {
		qfWarning() << "Cannot open style sheet:" << css_file_name;
	}
}

void Application::releaseQmlEngine()
{
	if(m_qmlEngine && m_qmlEngine->parent() == this) {
		/// it is safer to delete qml engine prior the application
		delete m_qmlEngine;
	}
	m_qmlEngine = nullptr;
}
