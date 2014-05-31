#include "mainwindow.h"
#include "frame.h"
#include "label.h"
#include <QApplication>
#include <QPushButton>
#include <QLayout>
#include <QtQml>

/*
void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
	char type_c = '\0';
	switch(type) {
	case QtDebugMsg:
		type_c = 'D';
		break;
	case QtWarningMsg:
		type_c = 'W';
		break;
	case QtCriticalMsg:
		type_c = 'E';
		break;
	case QtFatalMsg: {
		if(msg.contains(QLatin1Literal("ASSERT: \"!d->isWidget\""))) {
			break;
		}
		if(msg.contains(QLatin1Literal("ASSERT"))) {
			break;
		}
		type_c = '!';
		break;
	}
	}
	if(type_c) {
		QByteArray localMsg = msg.toLocal8Bit();
		fprintf(stderr, "[%c] (%s:%u %s): %s\n", type_c, context.file, context.line, context.function, localMsg.constData());
		fflush(stderr);
	}
}
*/
int main(int argc, char *argv[])
{
	//qInstallMessageHandler(myMessageOutput);

	qmlRegisterType<Label>("mywidgets", 1, 0, "Label");
	qmlRegisterType<QPushButton>("mywidgets", 1, 0, "Button");
	qmlRegisterType<Frame>("mywidgets", 1, 0, "Frame");
	qmlRegisterType<QWidget>("mywidgets", 1, 0, "Widget");
	qmlRegisterType<QVBoxLayout>("mywidgets", 1, 0, "VBoxLayout");

	qDebug() << "creating application instance";
	//qFatal("ASSERT");

	QApplication a(argc, argv);

	QQmlEngine *engine = new QQmlEngine;
	QQmlComponent component(engine, QUrl::fromLocalFile("main.qml"));
	if(!component.isReady()) {
		qDebug() << component.errorString();
	}
	else {
		QWidget *root = qobject_cast<QWidget*>(component.create());
		if(root)
			root->show();

		//MainWindow w;
		//w.show();

		return a.exec();
	}
}
