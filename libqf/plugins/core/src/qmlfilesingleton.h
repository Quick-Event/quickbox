#ifndef QF_CORE_QML_QMLFILESINGLETON_H
#define QF_CORE_QML_QMLFILESINGLETON_H

#include <QObject>
#include <QUrl>

class QQmlEngine;
class QJSEngine;

namespace qf {
namespace core {
namespace qml {

class QmlFileSingleton : public QObject
{
	Q_OBJECT
public:
	explicit QmlFileSingleton(QObject *parent = nullptr);
	~QmlFileSingleton() Q_DECL_OVERRIDE;

	static QObject* singletontype_provider(QQmlEngine *engine, QJSEngine *scriptEngine);

	Q_INVOKABLE bool write(const QString &file_path, const QString &content);
	Q_INVOKABLE bool writeHtml(const QString &file_path, const QVariant &body_list, const QVariantMap &options);
	Q_INVOKABLE bool writeXml(const QString &file_path, const QVariant &body_list, const QVariantMap &options);

	Q_INVOKABLE QString toAscii7(const QString &s, bool to_lower = false);
	Q_INVOKABLE QString tempPath();
	Q_INVOKABLE bool mkpath(const QString &absolute_dir_path);
	Q_INVOKABLE QUrl toUrl(const QString &file_path);
	Q_INVOKABLE void openUrl(const QUrl &url);
};

} // namespace qml
} // namespace core
} // namespace qf

#endif // QF_CORE_QML_QMLFILESINGLETON_H
