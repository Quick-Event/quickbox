#include "plugin.h"
#include "application.h"
#include "mainwindow.h"

#include <qf/core/utils.h>
#include <qf/core/log.h>
#include <qf/core/assert.h>

#include <QQmlEngine>
#include <QTranslator>

using namespace qf::qmlwidgets::framework;

Plugin::Plugin(const QString &feature_id, QObject *parent)
	: QObject(parent)
{
	qfLogFuncFrame();
	m_featureId = feature_id;
	loadTranslations();
}

Plugin::Plugin(QObject *parent)
	: QObject(parent)
{
	qfLogFuncFrame();
}

Plugin::~Plugin()
{
	qfLogFuncFrame() << this;
}

void Plugin::loadTranslations()
{
	auto mainWindow = qobject_cast<MainWindow*>(this->parent());
	QString lc_name = mainWindow->uiLanguageName();
	if(!lc_name.isEmpty()) {
		QString tr_name = featureId() + '.' + lc_name;
		QString app_translations_path = QCoreApplication::applicationDirPath() + "/translations";
		QTranslator *trans = new QTranslator(mainWindow);
		bool ok = trans->load(tr_name, app_translations_path);
		if(ok) {
			qfInfo() << "Found translation file for:" << tr_name;
			QCoreApplication::instance()->installTranslator(trans);
		}
		else {
			qfInfo() << "Cannot load translation file for:" << tr_name << "in:" << app_translations_path;
			delete trans;
		}
	}
}

QQmlEngine *Plugin::qmlEngine()
{
	QQmlEngine *qe = Application::instance()->qmlEngine();
	QF_ASSERT(qe != nullptr, "Qml engine is NULL", return nullptr);
	return qe;
}
