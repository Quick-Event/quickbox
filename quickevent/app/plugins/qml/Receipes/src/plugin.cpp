#include "Receipes/receipesplugin.h"

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
		Q_ASSERT(uri == QLatin1String("Receipes"));

		qmlRegisterType<Receipes::ReceipesPlugin>(uri, 1, 0, "ReceipesPlugin");
	}
};

#include "plugin.moc"
