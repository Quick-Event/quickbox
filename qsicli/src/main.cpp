#include "theapp.h"
#include "mainwindow.h"

//#include <exception>

#include <qf/core/logcust.h>

void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
	QByteArray localMsg = msg.toLocal8Bit();
	switch (type) {
	case QtDebugMsg:
		fprintf(stderr, "Debug: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
		break;
	case QtWarningMsg:
		fprintf(stderr, "Warning: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
		break;
	case QtCriticalMsg:
		fprintf(stderr, "Critical: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
		break;
	case QtFatalMsg:
		fprintf(stderr, "Fatal: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
		break;
	}
}
#if 0
extern "C" void abort(void);

void abort(void) throw()
{
	qfError() << "myabort called";
	// Do whatever it takes, DebugStr or whatever...
	//return;
}

void myterminate() {
	qfError() << "myterminate handler called";
}
void myunexpected() {
	qfError() << "myunexpected handler called";
}
#endif
int main(int argc, char* argv[])
{
	//std::set_unexpected(myunexpected);
	//std::set_terminate(myterminate);

	qf::core::Log::setDefaultLogTreshold(qf::core::Log::LOG_INF);
	qf::core::Log::setDomainTresholds(argc, argv);

	qfError() << "QFLog(ERROR) test OK.";
	qfWarning() << "QFLog(WARNING) test OK.";
	qfInfo() << "QFLog(INFO) test OK.";
	qfDebug() << "QFLog(DEBUG) test OK.";
	qfTrash() << "QFLog(TRASH) test OK.";

	qInstallMessageHandler(myMessageOutput);
	TheApp app(argc, argv);
	QCoreApplication::setOrganizationName("OrienteeringTools");
	QCoreApplication::setOrganizationDomain("sourceforge.net");
	QCoreApplication::setApplicationName("QSICli");

	//qf::core::Log::setDefaultLogTreshold(app.logLevelFromSettings());

	//Q_INIT_RESOURCE(qsicli);
	MainWindow w;
	//qDebug() << "showing main window";
	w.show();
	//qDebug() << "enterring the message loop";
	int ret = app.exec();
	qfInfo() << "bye ...";
	return ret;
}
