#include <QQmlExtensionPlugin>
#include <qqml.h>
#include <QThread>

#include <qf/core/log.h>

class QmlLogSingleton : public QObject
{
	Q_OBJECT
public:
	QmlLogSingleton(QObject* parent = 0)
		: QObject(parent)
	{
	}

	~QmlLogSingleton() {}

	Q_INVOKABLE void debug(const QString &msg) { qfDebug() << msg; }
	Q_INVOKABLE void info(const QString &msg) { qfInfo() << QThread::currentThread() << msg; }
	Q_INVOKABLE void warning(const QString &msg) { qfWarning() << msg; }
	Q_INVOKABLE void error(const QString &msg) { qfError() << msg; }
	Q_INVOKABLE void fatal(const QString &msg) { qfFatal("%s", qPrintable(msg)); }
};

// Second, define the singleton type provider function (callback).
static QObject *qmllog_singletontype_provider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
	Q_UNUSED(engine)
	Q_UNUSED(scriptEngine)

	QmlLogSingleton *s = new QmlLogSingleton();
	return s;
}

class QmlPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")
public:
    void registerTypes(const char *uri)
    {
		qfLogFuncFrame() << uri;
		Q_ASSERT(uri == QLatin1String("qf.core"));
		//qmlRegisterType<QObject>(uri, 1, 0, "MyOu");
		qmlRegisterSingletonType<QmlLogSingleton>(uri, 1, 0, "Log_helper", qmllog_singletontype_provider);
    }
};

#include "plugin.moc"
