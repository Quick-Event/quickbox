#include "qmlwidgetssingleton.h"
#include "inputdialogsingleton.h"
#include "messageboxsingleton.h"

#include <qf/qmlwidgets/frame.h>
#include <qf/qmlwidgets/label.h>
#include <qf/qmlwidgets/lineedit.h>
#include <qf/qmlwidgets/splitter.h>
#include <qf/qmlwidgets/statusbar.h>
#include <qf/qmlwidgets/action.h>
#include <qf/qmlwidgets/progressbar.h>
#include <qf/qmlwidgets/menubar.h>
#include <qf/qmlwidgets/dialogs/dialog.h>
#include <qf/qmlwidgets/dialogs/inputdialog.h>
#include <qf/qmlwidgets/dialogs/messagebox.h>
#include <qf/qmlwidgets/dialogbuttonbox.h>
#include <qf/qmlwidgets/tableview.h>
#include <qf/qmlwidgets/tableviewtoolbar.h>

#include <qf/qmlwidgets/layoutpropertiesattached.h>
#include <qf/qmlwidgets/gridlayoutproperties.h>

#include <qf/qmlwidgets/framework/plugin.h>
#include <qf/qmlwidgets/framework/pluginmanifest.h>
#include <qf/qmlwidgets/framework/dockwidget.h>
#include <qf/qmlwidgets/framework/partwidget.h>

#include <qf/core/log.h>

#include <QPushButton>
#include <QFrame>
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
		Q_ASSERT(uri == QLatin1String("qf.qmlwidgets"));

		qmlRegisterSingletonType<QmlWidgetsSingleton>(uri, 1, 0, "QmlWidgetsSingleton", &QmlWidgetsSingleton::singletontype_provider);
		qmlRegisterSingletonType<MessageBoxSingleton>(uri, 1, 0, "MessageBoxSingleton", &MessageBoxSingleton::singletontype_provider);
		qmlRegisterSingletonType<InputDialogSingleton>(uri, 1, 0, "InputDialogSingleton", &InputDialogSingleton::singletontype_provider);

		// QWidget have to be registered to make QQmlListProperty<QWidget> work !!!
		// in other case strange error:
		// [Invalid property assignment: "widgets" is a read-only property]
		// is issued
		qmlRegisterType<QWidget>(uri, 1, 0, "QWidget");
		qmlRegisterType<QFrame>(uri, 1, 0, "QFrame");

		qmlRegisterType<qf::qmlwidgets::Label>(uri, 1, 0, "Label");
		qmlRegisterType<qf::qmlwidgets::LineEdit>(uri, 1, 0, "LineEdit");
		qmlRegisterType<qf::qmlwidgets::Splitter>(uri, 1, 0, "Splitter");
		qmlRegisterType<qf::qmlwidgets::ProgressBar>(uri, 1, 0, "ProgressBar");
		qmlRegisterType<QPushButton>(uri, 1, 0, "Button");
		qmlRegisterType<qf::qmlwidgets::Frame>(uri, 1, 0, "Frame");
		qmlRegisterType<qf::qmlwidgets::TableView>(uri, 1, 0, "TableView");
		qmlRegisterType<qf::qmlwidgets::TableViewToolBar>(uri, 1, 0, "TableViewToolBar");

		qmlRegisterType<qf::qmlwidgets::Action>(uri, 1, 0, "Action");
		// Menu is not needed for now
		//qmlRegisterType<qf::qmlwidgets::Menu>(uri, 1, 0, "Menu");
		qmlRegisterType<qf::qmlwidgets::MenuBar>(uri, 1, 0, "MenuBar");
		qmlRegisterType<qf::qmlwidgets::StatusBar>(uri, 1, 0, "StatusBar");

		qmlRegisterType<qf::qmlwidgets::framework::Plugin>(uri, 1, 0, "Plugin");
		qmlRegisterType<qf::qmlwidgets::framework::PluginManifest>(uri, 1, 0, "PluginManifest");
		qmlRegisterType<qf::qmlwidgets::framework::DockWidget>(uri, 1, 0, "DockWidget");
		qmlRegisterType<qf::qmlwidgets::framework::PartWidget>(uri, 1, 0, "PartWidget");

		qmlRegisterType<qf::qmlwidgets::DialogButtonBox>(uri, 1, 0, "ButtonBox");

		qmlRegisterType<qf::qmlwidgets::LayoutPropertiesAttached>();
		qmlRegisterType<qf::qmlwidgets::LayoutProperties>(uri, 1, 0, "Layout");

		qmlRegisterType<qf::qmlwidgets::GridLayoutProperties>(uri, 1, 0, "GridLayoutProperties");

		qmlRegisterType<qf::qmlwidgets::dialogs::Dialog>(uri, 1, 0, "Dialog");
		qmlRegisterType<qf::qmlwidgets::dialogs::InputDialog>(uri, 1, 0, "InputDialog");
		qmlRegisterType<qf::qmlwidgets::dialogs::MessageBox>(uri, 1, 0, "MessageBox");
	}
};

#include "plugin.moc"
