#ifndef QF_CORE_QML_QMLFILESINGLETON_H
#define QF_CORE_QML_QMLFILESINGLETON_H

#include <QObject>

class QQmlEngine;
class QJSEngine;

namespace qf {
namespace core {
namespace qml {

class QmlFileSingleton : public QObject
{
	Q_OBJECT
public:
	explicit QmlFileSingleton(QObject *parent = 0);
	~QmlFileSingleton() Q_DECL_OVERRIDE;

	static QObject* singletontype_provider(QQmlEngine *engine, QJSEngine *scriptEngine);

	Q_INVOKABLE bool write(const QString &file_path, const QString &content);
	Q_INVOKABLE bool writeHtml(const QString &file_path, const QVariant &body_list, const QVariantMap &options);
};

} // namespace qml
} // namespace core
} // namespace qf

#endif // QF_CORE_QML_QMLFILESINGLETON_H
