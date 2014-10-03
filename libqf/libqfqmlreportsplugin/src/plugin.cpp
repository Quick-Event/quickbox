#include "qmlreportssingleton.h"

// #include <qf/qmlwidgets/frame.h>
// #include <qf/qmlwidgets/label.h>
// #include <qf/qmlwidgets/lineedit.h>
// #include <qf/qmlwidgets/splitter.h>
// #include <qf/qmlwidgets/statusbar.h>
// #include <qf/qmlwidgets/action.h>
// #include <qf/qmlwidgets/progressbar.h>
// #include <qf/qmlwidgets/menubar.h>
// #include <qf/qmlwidgets/dialogs/qmldialog.h>
// #include <qf/qmlwidgets/dialogs/inputdialog.h>
// #include <qf/qmlwidgets/dialogs/messagebox.h>
// #include <qf/qmlwidgets/dialogbuttonbox.h>
// #include <qf/qmlwidgets/tableview.h>
// #include <qf/qmlwidgets/tableviewtoolbar.h>
// 
// #include <qf/qmlwidgets/layoutpropertiesattached.h>
// #include <qf/qmlwidgets/layouttypeproperties.h>

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

		///qmlRegisterType<qf::qmlreports::dialogs::MessageBox>(uri, 1, 0, "MessageBox");
	}
};

#include "plugin.moc"
