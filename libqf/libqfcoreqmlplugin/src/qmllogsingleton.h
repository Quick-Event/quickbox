#ifndef QF_CORE_QML_QMLLOGSINGLETON_H
#define QF_CORE_QML_QMLLOGSINGLETON_H

#include <QObject>

class QQmlEngine;
class QJSEngine;

namespace qf {
namespace core {
namespace qml {

class QmlLogSingleton : public QObject
{
	Q_OBJECT
public:
	QmlLogSingleton(QObject* parent = 0);
	//~QmlLogSingleton() {}

	static QObject* singletontype_provider(QQmlEngine *engine, QJSEngine *scriptEngine);

	Q_INVOKABLE void debug(const QString &msg);
	Q_INVOKABLE void info(const QString &msg);
	Q_INVOKABLE void warning(const QString &msg);
	Q_INVOKABLE void error(const QString &msg);
	Q_INVOKABLE void fatal(const QString &msg);
};

}}}

#endif
