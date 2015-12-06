#include "logdevice.h"
#include "utils.h"

#include <QByteArray>
#include <QString>
#include <QDateTime>

#ifdef Q_OS_UNIX
#include <unistd.h>
#include <cstdio>
#endif


using namespace qf::core;

namespace {

Log::Level environment_treshold()
{
	const QByteArray ba = qgetenv("QF_LOG_TRESHOLD");
	if(ba.isEmpty())
		return Log::Level::Invalid;
	QString s = QString::fromLatin1(ba.data());
	bool ok;
	int ret =  s.toInt(&ok);
	if(!ok)
		return Log::Level::Invalid;
	return Log::Level(ret);
}

namespace {
QList< LogDevice* >& logDevices();
}

void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
	if(!LogDevice::isLoggingEnabled())
		return;
	Log::Level level = Log::Level::Debug;
	switch(type) {
	case QtDebugMsg:
		level = Log::Level::Debug;
		break;
#if (QT_VERSION >= QT_VERSION_CHECK(5, 5, 0))
	case QtInfoMsg:
		level = Log::Level::Info;
		break;
#endif
	case QtWarningMsg:
		/*
		if(QLatin1String(context.category) == Log::categoryDebugName)
			level = Log::Level::Debug;
		else if(QLatin1String(context.category) == Log::categoryInfoName)
			level = Log::Level::Info;
		else if(QLatin1String(context.category) == Log::categoryWarningName)
			level = Log::Level::Warning;
		else if(QLatin1String(context.category) == Log::categoryErrorName)
			level = Log::Level::Error;
		else if(QLatin1String(context.category) == Log::categoryFatalName)
			level = Log::Level::Fatal;
		else level = Log::Level::Warning;
		*/
		level = Log::Level::Warning;
		break;
	case QtCriticalMsg:
		level = Log::Level::Error;
		break;
	case QtFatalMsg:
		level = Log::Level::Fatal;
		break;
	}
	Q_FOREACH(auto log_device, logDevices()) {
		if(log_device->isEnabled() && log_device->checkLogPermisions(context, level)) {
			log_device->log(level, context, msg);
		}
	}
}

namespace {
QList< LogDevice* >& logDevices()
{
	static bool first_run = true;
	if(first_run) {
		qInstallMessageHandler(myMessageOutput);
	}
	static QList< LogDevice* > log_devices;
	return log_devices;
}
}

}

//=========================================================
// LogDevice
//=========================================================
Log::Level LogDevice::environmentLogTreshold = environment_treshold();
Log::Level LogDevice::commandLineLogTreshold = Log::Level::Invalid;

bool LogDevice::m_loggingEnabled = true;

LogDevice::LogDevice(QObject *parent)
	: QObject(parent)
	, m_logTreshold(Log::Level::Info)
	, m_count(0)
	//, m_isPrettyDomain(true)
{
}

LogDevice::~LogDevice()
{
	logDevices().removeOne(this);
}

QString LogDevice::moduleFromContext(const QMessageLogContext &context)
{
	QString ret = QString::fromUtf8(context.file);
	static QRegExp rx("(\\\\|\\/)");
	ret = ret.section(rx, -1);
	return ret;
}

void LogDevice::install(LogDevice *dev)
{
	logDevices() << dev;
}

Log::Level LogDevice::setLogTreshold(Log::Level level)
{
	Log::Level old = m_logTreshold;
	m_logTreshold = level;
	return old;
}

QStringList LogDevice::setModulesTresholds(int argc, char *argv[])
{
	QStringList ret;
	m_modulesTresholds.clear();
	QStringList tresholds;
	for(int i=0; i<argc; i++) {
		QString s = QString::fromUtf8(argv[i]);
		if(i == 0) {
			ret << s;
			continue;
		}
		if(s == QLatin1String("-d")) {
			i++;
			s = s.mid(2);
			tresholds << s;
		}
		else {
			ret << s;
		}
	}
	for(QString mod_tres : tresholds) {
		int ix = mod_tres.indexOf(':');
		QString module = mod_tres;
		//printf("domainTreshold %s\n", qPrintable(dom_tres));
		Log::Level level = Log::Level::Debug;
		if(ix > 0) {
			module = mod_tres.mid(0, ix);
			QString s = mod_tres.mid(ix + 1, 1);
			QChar c = s.isEmpty()? QChar(): s[0].toUpper();
			if(c == 'D')
				level = Log::Level::Debug;
			else if(c == 'I')
				level = Log::Level::Info;
			else if(c == 'W')
				level = Log::Level::Warning;
			else if(c == 'E')
				level = Log::Level::Error;
		}
		if(module.isEmpty()) {
			setLogTreshold(level);
		}
		else {
			//printf("add domainTreshold %s %d \n", qPrintable(domain), level);
			m_modulesTresholds[module] = level;
		}
	}
	return ret;
}

Log::Level LogDevice::logTreshold()
{
	if(commandLineLogTreshold > Log::Level::Invalid)
		return commandLineLogTreshold;
	if(environmentLogTreshold > Log::Level::Invalid)
		return environmentLogTreshold;
	return m_logTreshold;
}

bool LogDevice::checkLogPermisions(const QMessageLogContext &context, Log::Level _level)
{
	bool ret = false;
	do {
		if(_level <= Log::Level::Invalid) break;
		if(_level == Log::Level::Fatal) {
			ret = true;
			break;
		}
		if(_level == Log::Level::Debug) {
#ifdef QF_NO_DEBUG_LOG
			break;
#endif
		}
		bool module_level_found = false;
		QString module = moduleFromContext(context);
		QMapIterator<QString, Log::Level> it(m_modulesTresholds);
		while (it.hasNext()) {
			it.next();
			if(module.indexOf(it.key(), 0, Qt::CaseInsensitive) >= 0) {
				//printf("found '%s' in '%s' treshold: %d \n", qPrintable(it.key()), qPrintable(_domain), it.value());
				module_level_found = true;
				if(_level <= it.value())
					ret = true;
				break;
			}
		}
		if(!module_level_found) {
			if(_level <= logTreshold())
				ret = true;
		}
	} while(false);
	//printf("%s %d \n", qPrintable(_domain), ret);
	return ret;
}

void LogDevice::setLoggingEnabled(bool on)
{
	m_loggingEnabled = on;
}

bool LogDevice::isLoggingEnabled()
{
	return m_loggingEnabled;
}

void LogDevice::setEnabled(bool b)
{
	m_enabled = b;
}
/*
void LogDevice::setPrettyDomain(bool b)
{
	m_isPrettyDomain = b;
}

bool LogDevice::isPrettyDomain() const
{
	return m_isPrettyDomain;
}
*/
const char* LogDevice::dCommandLineSwitchHelp()
{
	return "-d[domain[:LEVEL]]\tset debug domain and level\n"
	"\t\t\tdomain: any substring of source module, for example 'mymod' prints debug info from every source file with name containing 'mymod', mymodule.cpp, tomymod.cpp, ...\n"
	"\t\t\tLEVEL: any of DEB, INFO, WARN, ERR, default level is INFO\n";
}
/*
QString LogDevice::prettyDomain(const QString &domain)
{
	if(!isPrettyDomain())
		return domain;
	static QRegExp rx("(\\\\|\\/)");
	return domain.section(rx, -1);
}
*/
//=========================================================
// FileLogDevice
//=========================================================

FileLogDevice::FileLogDevice(QObject *parent)
	: Super(parent)
{
	m_file = stderr;
}

FileLogDevice::~FileLogDevice()
{
	if(m_file != stderr) {
		::fclose(m_file);
	}
}

FileLogDevice *FileLogDevice::install()
{
	FileLogDevice *ret = new FileLogDevice();
	Super::install(ret);
	return ret;
}

void FileLogDevice::setFile(const QString &path_to_file)
{
	if(m_file && m_file != stderr) {
		::fclose(m_file);
	}
	if(path_to_file.isEmpty()) {
		m_file = stderr;
	}
	else {
		FILE *f = ::fopen(qPrintable(path_to_file), "w");
		if(f) {
			std::fprintf(stderr, "Redirecting log to file: %s\n", qPrintable(path_to_file));
			m_file = f;
		}
		else {
			std::fprintf(stderr, "Cannot open log file '%s' for writing\n", qPrintable(path_to_file));
		}
	}
}

void FileLogDevice::log(Log::Level level, const QMessageLogContext &context, const QString &msg)
{
	if(!m_file)
		return;
#ifdef Q_OS_UNIX
	bool is_tty = ::isatty(fileno(m_file));
	enum TerminalColor {Black = 0, Red, Green, Yellow, Blue, Magenta, Cyan, White};
	enum TerminalAttr {AttrReset = 0, AttrBright, AttrDim, AttrUnderline, AttrBlink, AttrReverse = 7, AttrHidden};
#endif

#ifdef Q_OS_UNIX
	if(is_tty) {
		TerminalColor fg, bg = Black;
		TerminalAttr attr = AttrReset;
		switch(level) {
		case Log::Level::Info:
			fg = Cyan; break;
		case Log::Level::Warning:
			fg = Magenta; attr = AttrBright; break;
		case Log::Level::Error:
		case Log::Level::Fatal:
			fg = Red; attr = AttrBright; break;
		case Log::Level::Debug:
		default:
			fg = White; break;
		}
		QString s;
		std::fprintf(m_file, "%c[%d;%d;%dm", 0x1B, attr, fg + 30, bg + 40);
	}
#endif
	std::fprintf(m_file, "%4i", ++m_count);
#ifdef Q_OS_UNIX
	if(!is_tty) {
		/// add timestamp if log goes to file
		QString s = QDateTime::currentDateTime().toString(Qt::ISODate);
		QByteArray ba = s.toLatin1();
		std::fprintf(m_file, " %s", ba.constData());
	}
#endif

	std::fprintf(m_file, "<%s>", Log::levelName(level));
	QString module = moduleFromContext(context);
	if(!module.isEmpty()) {
		std::fprintf(m_file, "[%s:%d] ", qPrintable(module), context.line);
	}
	std::fprintf(m_file, "%s", qPrintable(msg));
#ifdef Q_OS_UNIX
	if(is_tty) {
		QString s;
		s.sprintf("%c[%d;%d;%dm", 0x1B, AttrReset, White + 30, Black + 40);
		QByteArray ba = s.toLatin1();
		std::fprintf(m_file, "%s", ba.constData());
	}
#endif
#ifdef Q_OS_WIN
	std::fprintf(m_file, "\r\n");
#else
	std::fprintf(m_file, "\n");
#endif
	std::fflush(m_file);

	//if(level == Log::LOG_FATAL) std::terminate(); Qt will do it itself
}

//=========================================================
// LogEntryMap
//=========================================================
static const auto KeyLevel = QStringLiteral("level");
static const auto KeyDomain = QStringLiteral("domain");
static const auto KeyMessage = QStringLiteral("message");
static const auto KeyFile = QStringLiteral("file");
static const auto KeyLine = QStringLiteral("line");
static const auto KeyFunction = QStringLiteral("function");

LogEntryMap::LogEntryMap(Log::Level level, const QString &domain, const QString &message, const QString &file, int line, const QString &function)
{
	this->operator[](KeyLevel) = (int)level;
	this->operator[](KeyDomain) = domain;
	this->operator[](KeyMessage) = message;
	this->operator[](KeyFile) = file;
	this->operator[](KeyLine) = line;
	this->operator[](KeyFunction) = function;
}

Log::Level LogEntryMap::level() const
{
	return (Log::Level)value(KeyLevel).toInt();
}

QString LogEntryMap::levelStr() const
{
	return Log::levelName(level());
}

QString LogEntryMap::message() const
{
	return value(KeyMessage).toString();
}

QString LogEntryMap::domain() const
{
	return value(KeyDomain).toString();
}

QString LogEntryMap::file() const
{
	return value(KeyFile).toString();
}

int LogEntryMap::line() const
{
	return value(KeyLine).toInt();
}

QString LogEntryMap::function() const
{
	return value(KeyFunction).toString();
}

QString LogEntryMap::toString() const
{
	QString ret = "{";
	ret += "\"level\":" + QString::number((int)level()) + ", ";
	ret += "\"domain\":" + domain() + ", ";
	ret += "\"message\":" + message() + ", ";
	ret += "\"file\":" + file() + ", ";
	ret += "\"line\":" + QString::number(line()) + ", ";
	ret += "\"function\":" + function() + "}";
	return ret;
}

//=========================================================
// SignalLogDevice
//=========================================================
SignalLogDevice::SignalLogDevice(QObject *parent)
	: Super(parent)
{
	//qRegisterMetaType<qf::core::LogEntryMap>("qf::core::LogEntryMap");
	connect(this, &SignalLogDevice::__logEntry, this, &SignalLogDevice::logEntry, Qt::QueuedConnection);
}

SignalLogDevice::~SignalLogDevice()
{
}

SignalLogDevice *SignalLogDevice::install()
{
	SignalLogDevice *ret = new SignalLogDevice();
	Super::install(ret);
	return ret;
}

void SignalLogDevice::log(Log::Level level, const QMessageLogContext &context, const QString &msg)
{
	QString module = moduleFromContext(context);
	LogEntryMap m(level, module, msg, context.file, context.line, context.function);
	emit __logEntry(m);
}
