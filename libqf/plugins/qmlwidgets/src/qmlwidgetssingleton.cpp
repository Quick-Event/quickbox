#include "qmlwidgetssingleton.h"

#include <qf/qmlwidgets/reports/widgets/reportviewwidget.h>
#include <qf/qmlwidgets/framework/mainwindow.h>
#include <qf/qmlwidgets/dialogs/dialog.h>

#include <QQmlEngine>

QmlWidgetsSingleton::QmlWidgetsSingleton(QObject *parent)
	: QObject(parent)
{
}

QObject *QmlWidgetsSingleton::singletontype_provider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
	Q_UNUSED(scriptEngine)
	QmlWidgetsSingleton *s = new QmlWidgetsSingleton(engine);
	return s;
}

void QmlWidgetsSingleton::showReport(const QString &report_qml_file
									 , const QVariant &single_table_data
									 , const QString &window_title
									 , const QString &persistent_settings_id
									 , const QVariantMap &report_init_properties)
{
	qf::qmlwidgets::reports::ReportViewWidget::showReport(nullptr, report_qml_file, single_table_data, window_title, persistent_settings_id, report_init_properties);
}

void QmlWidgetsSingleton::showReport2(const QString &report_qml_file
									  , const QVariantMap &multiple_table_data
									  , const QString &window_title
									  , const QString &persistent_settings_id
									  , const QVariantMap &report_init_properties)
{
	qf::qmlwidgets::reports::ReportViewWidget::showReport2(nullptr, report_qml_file, multiple_table_data, window_title, persistent_settings_id, report_init_properties);
}


