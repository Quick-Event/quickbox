#include "theapp.h"
#include "mainwindow.h"

#include <qflog.h>
#include <qfcompat.h>
#include <qfstring.h>
#include <qfdlgexception.h>

//#include <QDialog>
#include <Qt>
#include <QMessageBox>
#include <QLocale>

//#define QF_LOG_NOT_MODULE_TRASH
#include <qflogcust.h>

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
void myMessageOutput(QtMsgType type, const char *msg)
{
	switch ( type ) {
		case QtDebugMsg:
			qfDebug() << msg;
			break;
		case QtWarningMsg:
			qfWarning() << msg;
			break;
		case QtCriticalMsg:
			qfError() << msg;
			break;
		case QtFatalMsg:
			qfFatal() << msg;
			//abort();                    // deliberately core dump
	}
}
#else
void myMessageOutput(QtMsgType, const QMessageLogContext &, const QString &)
{

}
#endif

void my_terminate()
{
	qfError() << "my_terminate: uncaught exception";
	qfError() << QFException::recentException().msg();
	qfError() << QFException::recentException().where();
	qfError() << QFException::recentException().stackTrace();
	QFDlgException::exec(NULL, QFException::recentException());
	abort();
}

int main(int argc, char *argv[])
{
    int ret = 0;

	std::set_terminate(my_terminate);

	QFException::setExceptionAbortsApplication(false);
	QFException::setAssertThrowsException(false);
	//QFLog::setLogTreshold(QFLog::LOG_TRASH);
	QFLog::setDefaultLogTreshold(QFLog::LOG_INF);
	#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
	qInstallMsgHandler(myMessageOutput);
	#else
	//Qt::qInstallMessageHandler(myMessageOutput);
	#endif

	QFLog::setDomainTresholds(argc, argv);
	//QFLog::domainTresholds()["connectionbase"] = QFLog::LOG_TRASH;
	//QFLog::domainTresholds()["catalog"] = QFLog::LOG_TRASH;
	//QFLog::domainTresholds()["qftablemodel"] = QFLog::LOG_TRASH;
	//QFLog::domainTresholds()["mainwindow"] = QFLog::LOG_TRASH;

	qfError() << "qfError() test OK.";
	qfWarning() << "qfWarning() test OK.";
	qfInfo() << "qfInfo() test OK.";
	qfDebug() << "qfDebug() test OK.";
	qfTrash() << "qfTrash() test OK.";

	//QLocale::setDefault(QLocale(QLocale::Czech, QLocale::CzechRepublic));
	//QLocale::setDefault(QLocale::system());
	/// defaultni locales jsou z LC_ALL
	qfInfo() << "Default locale set to:" << QLocale().name();

	/*
	qfTrash() << "c1";
	QFSqlConnection c1;
	qfTrash() << "c1.refCnt():" << c1.refCnt();
	qfTrash() << "c2(c1)";
	QFSqlConnection c2(c1);
	qfTrash() << "c1.refCnt():" << c1.refCnt();
	qfTrash() << "c2.refCnt():" << c2.refCnt();
	qfTrash() << "c3";
	QFSqlConnection c3;
	qfTrash() << "c3 = c2";
	c3 = c2;
	qfTrash() << "c3.refCnt():" << c3.refCnt();
	//qfTrash() << "c4";
	//QFSqlConnection c4;
	return 0;
	*/
	TheApp app(argc, argv); // have to be outside try block, because of QMessageBox

	try {
		app.redirectLog();

		MainWindow *mainWindow = new MainWindow();
		//app.setMainWidget(&mainWindow);
		mainWindow->show();
		ret = app.exec();
		delete mainWindow;
	}
	catch(QFException &e) {
		QFDlgException dlg; dlg.exec(e);
	}
	catch(std::exception &e) {
		QMessageBox::warning(NULL, "QtCriticalMsg", e.what());
		qfFatal() << "Unhandled exception:\n" << e.what();
	}
	catch(...) {
		qfFatal() << "Unhandled exception !";
	}
	qfDebug() << "bye ...";
	return ret;
}
