#include <qf/core/log.h>

#include <qf/core/stacktrace.h>

#include <QStringList>
#include <QDateTime>
#include <QCoreApplication>

#include <cstdlib>
#include <cstdio>
#include <sys/types.h>
#include <cstdarg>
#include <unistd.h>

using namespace qf::core;

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

//======================================================
//                                Log
//======================================================
const char *Log::levelNames[] = {"F", "E", "W", "I", "D", "T"};
LogDevice *Log::f_defaultLogDevice = NULL;

const char* Log::levelName(Log::Level _level)
{
	const char *ret = "?";
	int level = _level - 1;
	if(level>=0 && level<(int)(sizeof(levelNames)/sizeof(char*))) ret = levelNames[level];
	return ret;
}

/**
Tady se stala pomerne zajimava vec, puvodne jsem mel

LogDevice Log::appLogDevice;
LogDevice *Log::defaultLogDevice = &Log::appLogDevice;

to fungovalo skvele az do te doby, kdy jsem v inmonitoru instancioval QUiLoader, skutecne stacil radek

QUiLoader ldr;

na to, aby prestalo fungovat logovani, z nejakeho duvodu se fLogTreshold prepsal ze 4 na 0
kdyz jsem to chtel debuggovat, debugger mi napsal, ze Log::appLogDevice was optimized out, coz teda nechapu
vyresil jsem to tak, ze jsem to prepsal timto zpusobem, ktery dela to samy, ale optimalizace neprobehne
fakt nevim, kde byla chyba
*/
LogDevice* Log::defaultLogDevice()
{
	static FileLogDevice dev;
	LogDevice *ret = f_defaultLogDevice;
	if(!ret) ret = &dev;
	return ret;
}

LogDevice* Log::setDefaultLogDevice(LogDevice* dev)
{
	LogDevice *ret = f_defaultLogDevice;
	f_defaultLogDevice = dev;
	return ret;
}

void Log::redirectDefaultLogFile(FILE *new_log_stream)
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
	FileLogDevice *dev = dynamic_cast<FileLogDevice*>(defaultLogDevice());
	if(dev && dev->file != new_log_stream) {
		//printf("%s fileno: %i\n", QF_FUNC_NAME, fileno(dev->file));
		if(dev->file) std::fclose(dev->file);
		dev->file = new_log_stream;
		//printf("%s 2  fileno: %i\n", QF_FUNC_NAME, fileno(dev->file));
	}
}
/*
void Log::redir(FILE *log_fnc)
{
    if(log_fnc == NULL) logFnc = flog_fnc_default;
    else logFnc = log_fnc;
}
*/
void Log::setDomainTresholds(const QStringList & dom_tres_list)
{
	if(defaultLogDevice()) {
		defaultLogDevice()->domainTresholds.clear();
		foreach(QString dom_tres, dom_tres_list) {
			QStringList sl = dom_tres.split(':');
			if(sl.count() > 1) {
				QString s = sl[1].toUpper();
				/// TRASH | DEB | INF | WARN | ERR
				if(s == "TRASH") defaultLogDevice()->domainTresholds[sl[0]] = Log::LOG_TRASH;
				else if(s == "DEB") defaultLogDevice()->domainTresholds[sl[0]] = Log::LOG_DEB;
				else if(s == "INF") defaultLogDevice()->domainTresholds[sl[0]] = Log::LOG_INF;
				else if(s == "WARN") defaultLogDevice()->domainTresholds[sl[0]] = Log::LOG_WARN;
				else if(s == "ERR") defaultLogDevice()->domainTresholds[sl[0]] = Log::LOG_ERR;
			}
			else {
				defaultLogDevice()->domainTresholds[sl[0]] = Log::LOG_TRASH;
			}
		}
	}
}

QStringList Log::setDomainTresholds(int argc, char *argv[])
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
				if(s == "ERR") tr = Log::LOG_ERR;
				else if(s == "WARN") tr = Log::LOG_WARN;
				else if(s == "INF") tr = Log::LOG_INF;
				else if(s == "DEB") tr = Log::LOG_DEB;
				else if(s == "TRASH") tr = Log::LOG_TRASH;
				else tr = s.toInt();
				if(tr >= Log::LOG_ERR) {
					LogDevice::commandLineLogTreshold = tr;
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

QString Log::stackTrace()
{
	return StackTrace::stackTrace();
}

//======================================================
//                                LogDevice
//======================================================
int LogDevice::environmentLogTreshold = environment_treshold();
int LogDevice::commandLineLogTreshold = 0;

int LogDevice::setLogTreshold(int level)
{
	//fprintf(stderr, "set log treshold: %i\n", level);
	int old = m_logTreshold;
	m_logTreshold = level;
	return old;
}

int LogDevice::logTreshold()
{
	if(this == Log::defaultLogDevice()) {
		if(commandLineLogTreshold > 0) return commandLineLogTreshold;
		if(environmentLogTreshold > 0) return environmentLogTreshold;
		//fprintf(stderr, "level: f: %i c: %i e: %i\n", fLogTreshold, commandLineLogTreshold, environmentLogTreshold);
	}
	return m_logTreshold;
}

bool LogDevice::checkLogPermisions(const QString &_domain, int _level)
{
	bool ret = false;
	//fprintf(stderr, "level: %i log treshold: %i\n", _level,  logTreshold());
	do {
		if(_level == Log::LOG_NULL) break;
		if(_level == Log::LOG_FATAL) {ret = true; break;} /// LOG_FATAL nelze zakazat
		if(_level == Log::LOG_TRASH) {
#ifndef Log_DO_TRASH_LOG
			//break;
#endif // Log_DO_TRASH_LOG
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
FILE *FileLogDevice::file = stderr;

void FileLogDevice::log(const QString &domain, int _level, const QString &msg, int flags)
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
				if(_level <= Log::LOG_ERR) setColorForNextLog(Log::Red, Log::Black, Log::AttrBright);
				else if(_level == Log::LOG_WARN) setColorForNextLog(Log::Magenta, Log::Black, Log::AttrBright);
				else if(_level == Log::LOG_INF) setColorForNextLog(Log::Cyan, Log::Black, Log::AttrBright);
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

		std::fprintf(file, "<%s>", Log::levelName((Log::Level)_level));
		//else std::fprintf(file, "<%d>", _level);

		if(m_stackLevel > 0) std::fprintf(file, "(%i)", m_stackLevel);

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
				s.sprintf("%c[%d;%d;%dm", 0x1B, Log::AttrReset, Log::White + 30, Log::Black + 40);
				//s.sprintf("1B[%d;%d;%dm", Log::AttrReset, Log::White + 30, Log::Black + 40);
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
	if(_level == Log::LOG_FATAL) std::terminate();
}
