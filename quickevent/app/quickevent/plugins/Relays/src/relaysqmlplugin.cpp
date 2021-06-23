//#include "relaysplugin.h"

//#include <qf/core/log.h>

//#include <QtQml>
//#include <QtQml/qqmlextensionplugin.h>

//class QmlPlugin : public QQmlExtensionPlugin
//{
//	Q_OBJECT
//	Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")
//public:
//	void registerTypes(const char *uri)
//	{
//		qfLogFuncFrame() << uri;
//		Q_ASSERT(uri == QLatin1String("Relays"));

//		//qmlRegisterSingletonType<qf::core::qml::QmlLogSingleton>(uri, 1, 0, "Log_helper", &qf::core::qml::QmlLogSingleton::singletontype_provider);
//		qmlRegisterType<Relays::RelaysPlugin>(uri, 1, 0, "RelaysPlugin");
//	}
//};

//#include "relaysqmlplugin.moc"
