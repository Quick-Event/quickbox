#include "qmllogsingleton.h"
#include "crypt.h"
#include "settings.h"

#include "sql/qmlsqlsingleton.h"
#include "sql/sqlconnection.h"
#include "sql/sqlquery.h"
#include "sql/sqlrecord.h"
#include "sql/sqlquerybuilder.h"

#include "model/sqltablemodel.h"
#include "model/tablemodelcolumn.h"

#include "network/networkaccessmanager.h"
#include "network/networkreply.h"


#include <qf/core/model/sqltablemodel.h>

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

		qmlRegisterType<qf::core::qml::SqlConnection>(uri, 1, 0, "SqlConnection");
		qmlRegisterType<qf::core::qml::SqlQuery>(uri, 1, 0, "SqlQuery");
		qmlRegisterType<qf::core::qml::SqlRecord>(uri, 1, 0, "SqlRecord");
		qmlRegisterType<qf::core::qml::SqlQueryBuilder>(uri, 1, 0, "SqlQueryBuilder");
		qmlRegisterType<qf::core::qml::SqlTableModel>(uri, 1, 0, "SqlTableModel");
		qmlRegisterType<qf::core::model::TableModel>(uri, 1, 0, "TableModel");
		qmlRegisterType<qf::core::qml::TableModelColumn>(uri, 1, 0, "ModelColumn");

		qmlRegisterType<qf::core::qml::NetworkAccessManager>(uri, 1, 0, "NetworkAccessManager");
		qmlRegisterType<qf::core::qml::NetworkReply>(uri, 1, 0, "NetworkReply");
    }
};

#include "plugin.moc"
