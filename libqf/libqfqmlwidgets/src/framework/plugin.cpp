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

QQmlEngine *Plugin::qmlEngine()
{
	QQmlEngine *qe = Application::instance()->qmlEngine();
	QF_ASSERT(qe != nullptr, "Qml engine is NULL", return nullptr);
	return qe;
}
