#include "application.h"

#include <qf/core/log.h>
#include <qf/core/assert.h>

#include <QQmlEngine>

using namespace qf::qmlwidgets::framework;

Application::Application(int &argc, char **argv) :
	Super(argc, argv), m_qmlEngine(nullptr)
{
	{
		QString path;
	#ifdef Q_OS_UNIX
		path = QCoreApplication::applicationDirPath() + "/../lib/qml";
	#else
		path = QCoreApplication::applicationDirPath() + "/qml";
	#endif
		m_qmlLibraryImportPaths << path;
	}
	{
		QString path;
		path = QCoreApplication::applicationDirPath() + "/divers/" + QCoreApplication::applicationName() + "/plugins";
		m_qmlPluginImportPaths << path;
	}
}

Application::~Application()
{
	releaseQmlEngine();
}

QQmlEngine *Application::qmlEngine()
{
	if(!m_qmlEngine) {
		setQmlEngine(createQmlEngine());
	}
	return m_qmlEngine;
}

void Application::setQmlEngine(QQmlEngine *eng)
{
	releaseQmlEngine();
	m_qmlEngine = eng;
	setupQmlImportPaths();
	connect(m_qmlEngine, &QQmlEngine::warnings, this, &Application::onQmlError);
}

const QList<QQmlError> &Application::qmlErrorList()
{
	return m_qmlErrorList;
}

void Application::clearQmlErrorList()
{
	m_qmlErrorList.clear();
}

QQmlEngine *Application::createQmlEngine()
{
	QQmlEngine *ret = new QQmlEngine(this);
	return ret;
}

void Application::setupQmlImportPaths()
{
	QF_ASSERT(m_qmlEngine != nullptr, "", return);

	for(auto path : m_qmlLibraryImportPaths) {
		qfInfo() << "Adding QML library import path:" << path;
		m_qmlEngine->addImportPath(path);
	}
	for(auto path : m_qmlPluginImportPaths) {
		qfInfo() << "Adding QML plugin import path:" << path;
		m_qmlEngine->addImportPath(path);
	}
}

void Application::onQmlError(const QList<QQmlError> &qmlerror_list)
{
	for(auto err : qmlerror_list) {
		qfError() << "QML ERROR:" << err.toString();
	}
	m_qmlErrorList << qmlerror_list;
}

void Application::releaseQmlEngine()
{
	if(m_qmlEngine && m_qmlEngine->parent() == this) {
		/// it is safer to delete qml engine prior the application
		delete m_qmlEngine;
	}
	m_qmlEngine = nullptr;
}
