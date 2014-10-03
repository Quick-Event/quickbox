#include "qmlreportssingleton.h"

#include <qf/qmlwidgets/reports/processor/reportitem.h>
// #include <qf/qmlwidgets/dialogs/inputdialog.h>

// #include <qf/qmlwidgets/framework/plugin.h>
// #include <qf/qmlwidgets/framework/pluginmanifest.h>
// #include <qf/qmlwidgets/framework/dockwidget.h>
// #include <qf/qmlwidgets/framework/partwidget.h>

#include <qf/core/log.h>

// #include <QPushButton>
// #include <QFrame>
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
		Q_ASSERT(uri == QLatin1String("qf.qmlreports"));

		qmlRegisterSingletonType<QmlReportsSingleton>(uri, 1, 0, "QmlReportsSingleton", &QmlReportsSingleton::singletontype_provider);

		qmlRegisterType<qf::qmlwidgets::reports::ReportItemReport>(uri, 1, 0, "Report");
	}
};

#include "plugin.moc"
