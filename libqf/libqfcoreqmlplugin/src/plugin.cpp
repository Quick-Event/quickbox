#include "qmllogsingleton.h"
#include "qmlsqlsingleton.h"
#include "crypt.h"
#include "settings.h"
#include "sqldatabase.h"

#include <qf/core/log.h>

#include <QQmlExtensionPlugin>
#include <qqml.h>

class QmlPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")
public:
    void registerTypes(const char *uri)
    {
		qfLogFuncFrame() << uri;
		Q_ASSERT(uri == QLatin1String("qf.core"));

		qmlRegisterSingletonType<qf::core::qml::QmlLogSingleton>(uri, 1, 0, "Log_helper", &qf::core::qml::QmlLogSingleton::singletontype_provider);
		qmlRegisterSingletonType<qf::core::qml::QmlSqlSingleton>(uri, 1, 0, "Sql", &qf::core::qml::QmlSqlSingleton::singletontype_provider);

		qmlRegisterType<qf::core::qml::Settings>(uri, 1, 0, "Settings");
		qmlRegisterType<qf::core::qml::Crypt>(uri, 1, 0, "Crypt");

		qmlRegisterType<qf::core::qml::SqlDatabase>(uri, 1, 0, "SqlDatabase");
    }
};

#include "plugin.moc"
