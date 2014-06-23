#include "qmlwidgetssingleton.h"

#include <qf/qmlwidgets/frame.h>
#include <qf/qmlwidgets/label.h>
#include <qf/qmlwidgets/lineedit.h>
#include <qf/qmlwidgets/splitter.h>
#include <qf/qmlwidgets/action.h>
#include <qf/qmlwidgets/menu.h>
#include <qf/qmlwidgets/menubar.h>
#include <qf/qmlwidgets/dialog.h>

#include <qf/qmlwidgets/layoutpropertiesattached.h>
#include <qf/qmlwidgets/gridlayoutproperties.h>

#include <qf/core/log.h>

#include <QPushButton>
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

		// not needed for now, maybe in future
		//qmlRegisterSingletonType<QmlWidgetsSingleton>(uri, 1, 0, "QmlWidgets_Singleton", &QmlWidgetsSingleton::singletontype_provider);

		// QWidget have to be registered to make QQmlListProperty<QWidget> work !!!
		// in other case strange error: Invalid property assignment: "widgets" is a read-only property
		// is issued
		qmlRegisterType<QWidget>(uri, 1, 0, "Widget");

		qmlRegisterType<qf::qmlwidgets::Label>(uri, 1, 0, "Label");
		qmlRegisterType<qf::qmlwidgets::LineEdit>(uri, 1, 0, "LineEdit");
		qmlRegisterType<qf::qmlwidgets::Splitter>(uri, 1, 0, "Splitter");
		qmlRegisterType<QPushButton>(uri, 1, 0, "Button");
		qmlRegisterType<qf::qmlwidgets::Frame>(uri, 1, 0, "Frame");

		qmlRegisterType<qf::qmlwidgets::Action>(uri, 1, 0, "Action");
		qmlRegisterType<qf::qmlwidgets::Menu>(uri, 1, 0, "Menu");
		qmlRegisterType<qf::qmlwidgets::MenuBar>(uri, 1, 0, "MenuBar");

		qmlRegisterType<qf::qmlwidgets::Dialog>(uri, 1, 0, "Dialog");

		qmlRegisterType<qf::qmlwidgets::LayoutPropertiesAttached>();
		qmlRegisterType<qf::qmlwidgets::LayoutProperties>(uri, 1, 0, "Layout");

		qmlRegisterType<qf::qmlwidgets::GridLayoutProperties>(uri, 1, 0, "GridLayoutProperties");
	}
};

#include "plugin.moc"
