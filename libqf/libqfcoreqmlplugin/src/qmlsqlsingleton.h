#ifndef QF_CORE_QML_QMLSQLSINGLETON_H
#define QF_CORE_QML_QMLSQLSINGLETON_H

#include <QObject>

class QQmlEngine;
class QJSEngine;

namespace qf {
namespace core {
namespace qml {

class QmlSqlSingleton : public QObject
{
	Q_OBJECT
public:
	QmlSqlSingleton(QObject *parent = 0);

	static QObject* singletontype_provider(QQmlEngine *engine, QJSEngine *scriptEngine);
};

}}}

#endif
