#include "application.h"
#include "qmlapplicationviewer.h"

#include <QDeclarativeContext>
#include <QSettings>
#include <QDebug>

Q_DECL_EXPORT int main(int argc, char *argv[])
{
	QSettings::setDefaultFormat(QSettings::IniFormat);
	QCoreApplication::setOrganizationName("OrienteeringTools");
	QCoreApplication::setOrganizationDomain("sourceforge.net");
	QCoreApplication::setApplicationName("QSIShow");

	QScopedPointer<Application> app(new Application(argc, argv));

	QmlApplicationViewer viewer;
	viewer.rootContext()->setContextProperty("ctx_app", app.data());
	//viewer.rootContext()->setContextProperty("ctx_model", app.property("model"));
	viewer.setOrientation(QmlApplicationViewer::ScreenOrientationAuto);
	//QString profile = app->profile();
	//qDebug() << "Application profile:" << profile;
	//viewer.addImportPath("divers/qsishow/qml");
	//viewer.addImportPath("divers/qsishow/qml/" + profile);
	viewer.setMainQmlFile(QLatin1String("divers/qsishow/qml/main.qml"));
	viewer.showExpanded();

	return app->exec();
}
