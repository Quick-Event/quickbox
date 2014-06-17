#include <qf/qmlwidgets/frame.h>
#include <qf/qmlwidgets/label.h>

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
		qmlRegisterType<qf::qmlwidgets::Label>(uri, 1, 0, "Label");
		qmlRegisterType<QPushButton>(uri, 1, 0, "Button");
		qmlRegisterType<qf::qmlwidgets::Frame>(uri, 1, 0, "Frame");

		// QWidget have to be registered to make QQmlListProperty<QWidget> work !!!
		// in other case strange error: Invalid property assignment: "widgets" is a read-only property
		// is issued
		qmlRegisterType<QWidget>(uri, 1, 0, "Widget");

    }
};

#include "plugin.moc"
