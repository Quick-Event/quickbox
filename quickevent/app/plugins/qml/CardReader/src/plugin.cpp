#include "CardReader/cardreaderplugin.h"
#include "CardReader/cardchecker.h"

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
		Q_ASSERT(uri == QLatin1String("CardReader"));

		//qmlRegisterSingletonType<qf::core::qml::QmlLogSingleton>(uri, 1, 0, "Log_helper", &qf::core::qml::QmlLogSingleton::singletontype_provider);
		qmlRegisterType<CardReader::CardReaderPlugin>(uri, 1, 0, "CardReaderPlugin");
		qmlRegisterType<CardReader::CardChecker>(uri, 1, 0, "CardChecker");
	}
};

#include "plugin.moc"
