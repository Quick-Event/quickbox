#include "qfcore_plugin.h"

#include "crypt.h"
#include "settings.h"

#include "qmllogsingleton.h"
//#include "qmlfilesingleton.h"
#include "sql/qmlsqlsingleton.h"
#include "sql/sqlconnection.h"
#include "sql/sqlquery.h"
#include "sql/sqlrecord.h"
#include "sql/sqlquerybuilder.h"
#include "model/sqldatadocument.h"

#include "model/sqltablemodel.h"
#include "model/tablemodelcolumn.h"

//#include <qf/core/network/networkaccessmanager.h>
//#include <qf/core/network/networkreply.h>

//#include <qf/core/model/sqltablemodel.h>
//#include <qf/core/model/datadocument.h>

//#include <qf/core/log.h>

//#include <QQmlExtensionPlugin>
//#include <QQmlEngine>

#include <QtQml>

QT_BEGIN_NAMESPACE

void QFCorePlugin::registerTypes(const char *uri)
{
	//qfLogFuncFrame() << uri;
	Q_ASSERT(uri == QLatin1String("qf.core"));

	qmlRegisterSingletonType<qf::core::qml::QmlLogSingleton>(uri, 1, 0, "Log_helper", &qf::core::qml::QmlLogSingleton::singletontype_provider);
	//qmlRegisterSingletonType<qf::core::qml::QmlFileSingleton>(uri, 1, 0, "File", &qf::core::qml::QmlFileSingleton::singletontype_provider);
	qmlRegisterSingletonType<qf::core::qml::QmlSqlSingleton>(uri, 1, 0, "Sql", &qf::core::qml::QmlSqlSingleton::singletontype_provider);

	qmlRegisterType<qf::core::qml::SqlConnection>(uri, 1, 0, "SqlConnection");
	qmlRegisterType<qf::core::qml::SqlQuery>(uri, 1, 0, "SqlQuery");
	qmlRegisterType<qf::core::qml::SqlRecord>(uri, 1, 0, "SqlRecord");
	qmlRegisterType<qf::core::qml::SqlQueryBuilder>(uri, 1, 0, "SqlQueryBuilder");
	qmlRegisterType<qf::core::qml::SqlTableModel>(uri, 1, 0, "SqlTableModel");
	qmlRegisterType<qf::core::model::TableModel>(uri, 1, 0, "TableModel");
	qmlRegisterType<qf::core::qml::TableModelColumn>(uri, 1, 0, "ModelColumn");

	qmlRegisterType<qf::core::model::DataDocument>(uri, 1, 0, "DataDocument");
	qmlRegisterType<qf::core::qml::SqlDataDocument>(uri, 1, 0, "SqlDataDocument");

	qmlRegisterType<qf::core::qml::Settings>(uri, 1, 0, "Settings");
	qmlRegisterType<qf::core::qml::Crypt>(uri, 1, 0, "Crypt");

	//qmlRegisterType<qf::core::network::NetworkAccessManager>(uri, 1, 0, "NetworkAccessManager");
	//qmlRegisterType<qf::core::network::NetworkReply>(uri, 1, 0, "NetworkReply");

	// Auto-increment the import to stay in sync with ALL future QtQuick minor versions
	qmlRegisterModule(uri, 1, 0);
}

QT_END_NAMESPACE

