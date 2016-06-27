#include "oris.h"
#include "orisimporter.h"

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
		Q_ASSERT(uri == QLatin1String("Oris"));

		qmlRegisterType<Oris>(uri, 1, 0, "Oris");
		qmlRegisterType<OrisImporter>(uri, 1, 0, "OrisImporter");
	}
};

#include "plugin.moc"
