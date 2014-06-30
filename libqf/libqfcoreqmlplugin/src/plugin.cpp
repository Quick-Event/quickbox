#include "qmllogsingleton.h"
#include "crypt.h"
#include "settings.h"
#include "sql/qmlsqlsingleton.h"
#include "sql/sqldatabase.h"
#include "sql/sqlquery.h"
#include "sql/sqlrecord.h"

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
		qmlRegisterType<qf::core::qml::SqlQuery>(uri, 1, 0, "SqlQuery");
		qmlRegisterType<qf::core::qml::SqlRecord>(uri, 1, 0, "SqlRecord");
    }
};

#include "plugin.moc"
