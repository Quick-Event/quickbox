#include "qmlreportssingleton.h"

#include <qf/qmlwidgets/reports/processor/reportitem.h>
#include <qf/qmlwidgets/reports/processor/style/sheet.h>
#include <qf/qmlwidgets/reports/processor/style/color.h>
#include <qf/qmlwidgets/reports/processor/style/pen.h>

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

		qmlRegisterType<qf::qmlwidgets::reports::ReportItem>(uri, 1, 0, "Item");
		qmlRegisterType<qf::qmlwidgets::reports::ReportItemReport>(uri, 1, 0, "Report");
		qmlRegisterType<qf::qmlwidgets::reports::ReportItemFrame>(uri, 1, 0, "Frame");
		qmlRegisterType<qf::qmlwidgets::reports::ReportItemPara>(uri, 1, 0, "Para");
		qmlRegisterType<qf::qmlwidgets::reports::style::Sheet>(uri, 1, 0, "Style__Sheet");
		qmlRegisterType<qf::qmlwidgets::reports::style::Color>(uri, 1, 0, "Style__Color");
		qmlRegisterType<qf::qmlwidgets::reports::style::Pen>(uri, 1, 0, "Style__Pen");
		qmlRegisterType<qf::qmlwidgets::reports::style::Brush>(uri, 1, 0, "Style__Brush");
		qmlRegisterType<qf::qmlwidgets::reports::style::Font>(uri, 1, 0, "Style__Font");
		qmlRegisterType<qf::qmlwidgets::reports::style::Text>(uri, 1, 0, "Style__Text");
	}
};

#include "plugin.moc"
