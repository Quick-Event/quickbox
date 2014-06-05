#ifndef THEAPP_H
#define THEAPP_H

#include <qf/core/log.h>
//#include <qfscriptdriver.h>

#include <QApplication>

namespace siut {
class DeviceDriver;
}
class QSqlDatabase;
//class SICliScriptDriver;
//class QTextStream;
class QFile;

class TheApp : public QApplication
{
Q_OBJECT
protected:
	siut::DeviceDriver *f_siDriver;
	//SICliScriptDriver *f_scriptDriver;
	QTextStream *f_cardLog;
	QFile *f_cardLogFile;
protected:
	//bool isScriptDebuggerEnabled();
public:
	static TheApp* instance(bool throw_exc = true);
	siut::DeviceDriver *siDriver();
	//SICliScriptDriver *scriptDriver();
	QSqlDatabase sqlConnection();

	QTextStream& cardLog();
	void closeCardLog();
	//void appendCardLogLine(const QString &line);

	QString versionString();

	qf::core::Log::Level logLevelFromSettings();
	void emitLogRequest(int level, const QString &msg) {emit logRequest(level, msg);}
	//void emitLogRequestPre(int level, const QString &msg) {emit logRequestPre(level, msg);}
	Q_SLOT void connectSql(bool open);
signals:
	void logRequest(int level, const QString &msg);
	//void logRequestPre(int level, const QString &msg);
	void sqlConnected(bool connected);
	void cardReadOut(int card_id);
public:
	TheApp(int & argc, char ** argv);
    virtual ~TheApp();
};

inline TheApp* theApp() {return TheApp::instance();}

#endif // THEAPP_H
