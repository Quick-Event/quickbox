#include "theapp.h"
#include "mainwindow.h"

#include <qf/core/log.h>
#include <qf/core/logdevice.h>

#include <QQmlEngine>
#include <QQmlComponent>
#include <QQmlContext>
#include <QThread>

int main(int argc, char* argv[])
{
	QScopedPointer<qf::core::LogDevice> log_device(qf::core::FileLogDevice::install(argc, argv));
	log_device->setPrettyDomain(true);

	qfError() << QThread::currentThread() << "QFLog(ERROR) test OK.";
	qfWarning() << "QFLog(WARNING) test OK.";
	qfInfo() << "QFLog(INFO) test OK.";
	qfDebug() << "QFLog(DEBUG) test OK.";

	TheApp app(argc, argv);
	QCoreApplication::setOrganizationName("OrienteeringTools");
	QCoreApplication::setOrganizationDomain("sourceforge.net");
	QCoreApplication::setApplicationName("QSiCli");

	QQmlEngine engine;
	engine.addImportPath(QCoreApplication::applicationDirPath() + "/../qml");
	engine.rootContext()->setContextProperty("TheApp", &app);
	QUrl extensions_url = QUrl::fromLocalFile(QCoreApplication::applicationDirPath() + "/divers/qsicli/extensions/qml/init.qml");
	qfDebug() << "creating extensions on path:" << extensions_url.toString();
	QQmlComponent component(&engine, extensions_url);
	if(!component.isReady()) {
		qfError() << component.errorString();
	}
	else {
		QObject *extensions_root = qobject_cast<QWidget*>(component.create());
		qfDebug() << "extensions created" << extensions_root;
	}

	MainWindow w;
	//qDebug() << "showing main window";
	w.show();
	//qDebug() << "enterring the message loop";
	int ret = app.exec();
	qfInfo() << "bye ...";
	return ret;
}
