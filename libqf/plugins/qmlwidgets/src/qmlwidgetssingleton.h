#ifndef QMLWIDGETSSINGLETON_H
#define QMLWIDGETSSINGLETON_H

#include <QObject>
#include <QVariantMap>

class QQmlEngine;
class QJSEngine;

class QmlWidgetsSingleton : public QObject
{
	Q_OBJECT
public:
	QmlWidgetsSingleton(QObject* parent = 0);

	Q_INVOKABLE void showReport(const QString &report_qml_file, const QVariant &table_data
			, const QString &window_title = tr("Report preview")
			, const QString &config_persistent_id = QString()
			, const QVariantMap &report_init_properties = QVariantMap());
	Q_INVOKABLE void showReport2(const QString &report_qml_file, const QVariantMap &table_data
			, const QString &window_title = tr("Report preview")
			, const QString &persistent_settings_id = QString()
			, const QVariantMap &report_init_properties = QVariantMap());
public:
	static QObject* singletontype_provider(QQmlEngine *engine, QJSEngine *scriptEngine);
};

#endif // QMLWIDGETSSINGLETON_H
