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
									 , const QVariant &table_data
									 , const QString &window_title
									 , const QString &persistent_settings_id
									 , const QVariantMap &report_init_properties)
{
	QVariantMap m;
	m[QString()] = table_data;
	showReport2(report_qml_file, m, window_title, persistent_settings_id, report_init_properties);
}

void QmlWidgetsSingleton::showReport2(const QString &report_qml_file
									  , const QVariantMap &table_data
									  , const QString &window_title
									  , const QString &persistent_settings_id
									  , const QVariantMap &report_init_properties)
{
	auto *w = new qf::qmlwidgets::reports::ReportViewWidget();
	w->setWindowTitle(window_title);
	if(!persistent_settings_id.isEmpty())
		w->setPersistentSettingsId(persistent_settings_id);
	w->setReport(report_qml_file, report_init_properties);
	QMapIterator<QString, QVariant> it(table_data);
	while(it.hasNext()) {
		it.next();
		w->setTableData(it.key(), it.value());
	}
	auto *parent_widget = qf::qmlwidgets::framework::MainWindow::frameWork();
	qf::qmlwidgets::dialogs::Dialog dlg(parent_widget);
	dlg.setCentralWidget(w);
	dlg.exec();
}


