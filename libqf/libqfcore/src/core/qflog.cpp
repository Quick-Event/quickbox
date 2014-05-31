#include "qflog.h"

#include <qfstacktrace.h>

#include <QStringList>

#include <qdatetime.h>
#include <qcoreapplication.h>

#include <cstdlib>
#include <cstdio>
#include <sys/types.h>
#include <cstdarg>
#include <unistd.h>

static int environment_treshold()
{
	const QByteArray ba = qgetenv("QF_LOG_TRESHOLD");
	if(ba.isEmpty()) return -1;
	QString s = QString::fromLocal8Bit(ba.data());
	bool ok;
	int ret =  s.toInt(&ok);
	if(!ok) return -1;
	return ret;
}

//flog_fnc_t *QFLog::logFnc = flog_fnc_default;
//int QFLog::recursionLevel = 0;

//const int QFLog::LOG_LEVEL_MASK = 0xFF;
//const int QFLog::LOG_CONT = 0x1000;

//QList<QString> QFLog::fAllowedDomains;
//QList<QString> QFLog::fDenyiedDomains;
//======================================================
//                                QFLogDevice
//======================================================
int QFLogDevice::environmentLogTreshold = environment_treshold();
int QFLogDevice::commandLineLogTreshold = 0;

int QFLogDevice::setLogTreshold(int level)
{
	//fprintf(stderr, "set log treshold: %i\n", level);
	int old = fLogTreshold;
	fLogTreshold = level;
	return old;
}

int QFLogDevice::logTreshold()
{
	if(this == QFLog::defaultLogDevice()) {
		if(commandLineLogTreshold > 0) return commandLineLogTreshold;
		if(environmentLogTreshold > 0) return environmentLogTreshold;
		//fprintf(stderr, "level: f: %i c: %i e: %i\n", fLogTreshold, commandLineLogTreshold, environmentLogTreshold);
	}
	return fLogTreshold;
}

bool QFLogDevice::checkLogPermisions(const QString &_domain, int _level)
{
	bool ret = false;
	//fprintf(stderr, "level: %i log treshold: %i\n", _level,  logTreshold());
	do {
		if(_level == QFLog::LOG_NULL) break;
		if(_level == QFLog::LOG_FATAL) {ret = true; break;} /// LOG_FATAL nelze zakazat
		if(_level == QFLog::LOG_TRASH) {
#ifndef QFLOG_DO_TRASH_LOG
			//break;
#endif // QFLOG_DO_TRASH_LOG
		}
		QMapIterator<QString, int> it(domainTresholds);
		while (it.hasNext()) {
			it.next();
			if(_domain.indexOf(it.key(), 0, Qt::CaseInsensitive) >= 0) {
				if(_level <= it.value()) ret = true;
				break;
			}
		}
		if(_level <= logTreshold()) ret = true;
	} while(false);
	//fprintf(stderr, "\tret:%i!\n", ret);
	return ret;
}
//======================================================
//                                QFFileLogDevice
//======================================================
FILE *QFFileLogDevice::file = stderr;

void QFFileLogDevice::log(const QString &domain, int _level, const QString &msg, int flags)
{
	//static QByteArray dom_str;
	//static QByteArray msg_str;
	//printf("%s fileno: %i\n", QF_FUNC_NAME, fileno(file));
	Q_UNUSED(flags);
	do {
		if(!file) break;
		//if(!checkLogPermisions(domain, _level)) break;
#ifdef Q_OS_UNIX
		if(isatty(fileno(file))) {
			if(!colored) {
				if(_level <= QFLog::LOG_ERR) setColorForNextLog(QFLog::Red, QFLog::Black, QFLog::AttrBright);
				else if(_level == QFLog::LOG_WARN) setColorForNextLog(QFLog::Magenta, QFLog::Black, QFLog::AttrBright);
				else if(_level == QFLog::LOG_INF) setColorForNextLog(QFLog::Cyan, QFLog::Black, QFLog::AttrBright);
			}
			if(colored) {
				/// obarvi vystup pouze kdyz to jde na terminal
				QString s;
				//s.sprintf("***BARVA*");
				s.sprintf("%c[%d;%d;%dm", 0x1B, colorAttr, fgColor + 30, bgColor + 40);
				//s.sprintf("1B[%d;%d;%dm", colorAttr, fgColor + 30, bgColor + 40);
				QByteArray ba = s.toLatin1();
				std::fprintf(file, "%s", ba.constData());
			}
		}
#endif
		std::fprintf(file, "%4i", ++count);
#ifdef Q_OS_UNIX
		if(isatty(fileno(file))) {
		}
		else {
			{
				/// pokud se loguje do souboru, pridej timestamp
				QString s = QDateTime::currentDateTime().toString(Qt::ISODate);
				QByteArray ba = s.toLatin1();
				std::fprintf(file, " %s", ba.constData());
			}
		}
#endif

		std::fprintf(file, "<%s>", QFLog::levelName((QFLog::Level)_level));
		//else std::fprintf(file, "<%d>", _level);

		if(stackLevel > 0) std::fprintf(file, "(%i)", stackLevel);

		if(!domain.isEmpty()) {
			//dom_str = domain.toLatin1();
			//fprintf(file, "[%s] ", dom_str.data());
			std::fprintf(file, "[%s] ", qPrintable(domain));
		}
		//msg_str = msg.toLatin1();
		//fprintf(file, "%s", msg_str.data());
		std::fprintf(file, "%s", qPrintable(msg));
#ifdef Q_OS_UNIX
		if(isatty(fileno(file))) {
			if(colored) {
				QString s;
				//s.sprintf("***ODBARVA*");
				s.sprintf("%c[%d;%d;%dm", 0x1B, QFLog::AttrReset, QFLog::White + 30, QFLog::Black + 40);
				//s.sprintf("1B[%d;%d;%dm", QFLog::AttrReset, QFLog::White + 30, QFLog::Black + 40);
				QByteArray ba = s.toLatin1();
				std::fprintf(file, "%s", ba.constData());
			}
		}
#endif
		std::fprintf(file, "\n");
		std::fflush(file);
	}
	while(false);
	colored = false;
	if(_level == QFLog::LOG_FATAL) std::terminate();
}
//======================================================
//                                QFLog
//======================================================
const char *QFLog::levelNames[] = {"F", "E", "W", "I", "D", "T"};
QFLogDevice *QFLog::f_defaultLogDevice = NULL;

const char* QFLog::levelName(QFLog::Level _level)
{
	const char *ret = "?";
	int level = _level - 1;
	if(level>=0 && level<(int)(sizeof(levelNames)/sizeof(char*))) ret = levelNames[level];
	return ret;
}

/**
Tady se stala pomerne zajimava vec, puvodne jsem mel

QFLogDevice QFLog::appLogDevice;
QFLogDevice *QFLog::defaultLogDevice = &QFLog::appLogDevice;

to fungovalo skvele az do te doby, kdy jsem v inmonitoru instancioval QUiLoader, skutecne stacil radek

QUiLoader ldr;

na to, aby prestalo fungovat logovani, z nejakeho duvodu se fLogTreshold prepsal ze 4 na 0
kdyz jsem to chtel debuggovat, debugger mi napsal, ze QFLog::appLogDevice was optimized out, coz teda nechapu
vyresil jsem to tak, ze jsem to prepsal timto zpusobem, ktery dela to samy, ale optimalizace neprobehne
fakt nevim, kde byla chyba
*/
QFLogDevice* QFLog::defaultLogDevice()
{
	static QFFileLogDevice dev;
	QFLogDevice *ret = f_defaultLogDevice;
	if(!ret) ret = &dev;
	return ret;
}

QFLogDevice* QFLog::setDefaultLogDevice(QFLogDevice* dev)
{
	QFLogDevice *ret = f_defaultLogDevice;
	f_defaultLogDevice = dev;
	return ret;
}

void QFLog::redirectDefaultLogFile(FILE *new_log_stream)
{
	if(new_log_stream == NULL) {
		static const QString LOG_FILE_FLAG = "--log-file=";
		QString fn;
		foreach(fn, QCoreApplication::arguments()){
			if(fn.startsWith(LOG_FILE_FLAG)) {
				fn = fn.mid(LOG_FILE_FLAG.length());
				break;
			}
			fn = QString();
		}
		if(!fn.isEmpty()) {
			QByteArray ba = fn.toLatin1();
			new_log_stream = std::fopen(ba.constData(), "wb");
			//if(new_log_stream) qfInfo() << "Default log redirected to:" << fn;
		}
		if(!new_log_stream) new_log_stream = stderr;
	}
	//printf("%s fileno new_log_stream: %i\n", QF_FUNC_NAME, fileno(new_log_stream));
	QFFileLogDevice *dev = dynamic_cast<QFFileLogDevice*>(defaultLogDevice());
	if(dev && dev->file != new_log_stream) {
		//printf("%s fileno: %i\n", QF_FUNC_NAME, fileno(dev->file));
		if(dev->file) std::fclose(dev->file);
		dev->file = new_log_stream;
		//printf("%s 2  fileno: %i\n", QF_FUNC_NAME, fileno(dev->file));
	}
}
/*
void QFLog::redir(FILE *log_fnc)
{
    if(log_fnc == NULL) logFnc = flog_fnc_default;
    else logFnc = log_fnc;
}
*/
void QFLog::setDomainTresholds(const QStringList & dom_tres_list)
{
	if(defaultLogDevice()) {
		defaultLogDevice()->domainTresholds.clear();
		foreach(QString dom_tres, dom_tres_list) {
			QStringList sl = dom_tres.split(':');
			if(sl.count() > 1) {
				QString s = sl[1].toUpper();
				/// TRASH | DEB | INF | WARN | ERR
				if(s == "TRASH") defaultLogDevice()->domainTresholds[sl[0]] = QFLog::LOG_TRASH;
				else if(s == "DEB") defaultLogDevice()->domainTresholds[sl[0]] = QFLog::LOG_DEB;
				else if(s == "INF") defaultLogDevice()->domainTresholds[sl[0]] = QFLog::LOG_INF;
				else if(s == "WARN") defaultLogDevice()->domainTresholds[sl[0]] = QFLog::LOG_WARN;
				else if(s == "ERR") defaultLogDevice()->domainTresholds[sl[0]] = QFLog::LOG_ERR;
			}
			else {
				defaultLogDevice()->domainTresholds[sl[0]] = QFLog::LOG_TRASH;
			}
		}
	}
}

QStringList QFLog::setDomainTresholds(int argc, char *argv[])
{
	QStringList args;
	QStringList tresholds;
	if(defaultLogDevice()) {
		for(int i=1; i<argc; i++) {
		//printf("########## %s\n", argv[i]);
			QString s = argv[i];
			if(s.startsWith("-d")) {
				s = s.mid(2);
				tresholds << s;
			}
			else if(s.startsWith("-g")) {
				s = s.mid(2).toUpper();
				int tr;
				if(s == "ERR") tr = QFLog::LOG_ERR;
				else if(s == "WARN") tr = QFLog::LOG_WARN;
				else if(s == "INF") tr = QFLog::LOG_INF;
				else if(s == "DEB") tr = QFLog::LOG_DEB;
				else if(s == "TRASH") tr = QFLog::LOG_TRASH;
				else tr = s.toInt();
				if(tr >= QFLog::LOG_ERR) {
					QFLogDevice::commandLineLogTreshold = tr;
				}
			}
			else {
				args << s;
			}
		}
	}
	setDomainTresholds(tresholds);
	return args;
}

QString QFLog::stackTrace()
{
	return QFStackTrace::stackTrace();
}

