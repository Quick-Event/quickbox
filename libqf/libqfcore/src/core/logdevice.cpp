#include "logdevice.h"

#include <QByteArray>
#include <QString>
#include <QDateTime>

#ifdef Q_OS_UNIX
#include <unistd.h>
#endif

using namespace qf::core;

namespace {

Log::Level environment_treshold()
{
	const QByteArray ba = qgetenv("QF_LOG_TRESHOLD");
	if(ba.isEmpty()) return Log::LOG_INVALID;
	QString s = QString::fromLatin1(ba.data());
	bool ok;
	int ret =  s.toInt(&ok);
	if(!ok) return Log::LOG_INVALID;
	return (Log::Level)ret;
}

QList< LogDevice* >& logDevices();

void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
	Log::Level level = Log::LOG_DEB;
	switch(type) {
	case QtDebugMsg: level = Log::LOG_DEB; break;
	case QtWarningMsg:
		if(QLatin1String(context.category) == Log::categoryDebugName) level = Log::LOG_DEB;
		else if(QLatin1String(context.category) == Log::categoryInfoName) level = Log::LOG_INFO;
		else if(QLatin1String(context.category) == Log::categoryWarningName) level = Log::LOG_WARN;
		else if(QLatin1String(context.category) == Log::categoryErrorName) level = Log::LOG_ERR;
		else if(QLatin1String(context.category) == Log::categoryFatalName) level = Log::LOG_FATAL;
		else level = Log::LOG_WARN;
		break;
	case QtCriticalMsg: level = Log::LOG_ERR; break;
	case QtFatalMsg: level = Log::LOG_FATAL; break;
	}
	for(auto log_device : logDevices()) {
		if(log_device->checkLogPermisions(LogDevice::domainFromContext(context), level)) {
			log_device->log(level, context, msg);
		}
	}
}

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

//=========================================================
// LogDevice
//=========================================================
Log::Level LogDevice::environmentLogTreshold = environment_treshold();
Log::Level LogDevice::commandLineLogTreshold = Log::LOG_INVALID;

LogDevice::LogDevice(QObject *parent)
	: QObject(parent), m_logTreshold(Log::LOG_INFO), m_count(0), m_isPrettyDomain(false)
{
	//setDomainTresholds(parent, argv);
}

LogDevice::~LogDevice()
{
	logDevices().removeOne(this);
}

QString LogDevice::domainFromContext(const QMessageLogContext &context)
{
	return QString::fromUtf8(context.file);
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

QStringList LogDevice::setDomainTresholds(int argc, char *argv[])
{
	QStringList ret;
	m_domainTresholds.clear();
	QStringList tresholds;
	for(int i=0; i<argc; i++) {
		QString s = QString::fromUtf8(argv[i]);
		if(i == 0) {
			ret << s;
			continue;
		}
		if(s.startsWith("-d")) {
			//printf("1 %s\n", qPrintable(s));
			s = s.mid(2);
			//printf("2 %s\n", qPrintable(s));
			tresholds << s;
		}
		else {
			ret << s;
		}
	}
	for(QString dom_tres : tresholds) {
		int ix = dom_tres.indexOf(':');
		QString domain = dom_tres;
		//printf("domainTreshold %s\n", qPrintable(dom_tres));
		Log::Level level = Log::LOG_DEB;
		if(ix > 0) {
			domain = dom_tres.mid(0, ix);
			QString s = dom_tres.mid(ix + 1).toLower();
			if(s == "DEB") level = Log::LOG_DEB;
			else if(s == "INFO") level = Log::LOG_INFO;
			else if(s == "WARN") level = Log::LOG_WARN;
			else if(s == "ERR") level = Log::LOG_ERR;
		}
		if(domain.isEmpty()) {
			setLogTreshold(level);
		}
		else {
			//printf("add domainTreshold %s %d \n", qPrintable(domain), level);
			m_domainTresholds[domain] = level;
		}
	}
	return ret;
}

Log::Level LogDevice::logTreshold()
{
	if(commandLineLogTreshold >= 0)
		return commandLineLogTreshold;
	if(environmentLogTreshold >= 0)
		return environmentLogTreshold;
	return m_logTreshold;
}

bool LogDevice::checkLogPermisions(const QString &_domain, Log::Level _level)
{
	bool ret = false;
	do {
		if(_level < 0) break;
		if(_level == Log::LOG_FATAL) {ret = true; break;}
		if(_level == Log::LOG_DEB) {
#ifdef QF_NO_DEBUG_LOG
			break;
#endif
		}
		bool domain_level_found = false;
		QMapIterator<QString, int> it(m_domainTresholds);
		while (it.hasNext()) {
			it.next();
			if(_domain.indexOf(it.key(), 0, Qt::CaseInsensitive) >= 0) {
				//printf("found %s %d \n", qPrintable(_domain), it.value());
				domain_level_found = true;
				if(_level >= it.value()) ret = true;
				break;
			}
		}
		if(!domain_level_found) {
			if(_level >= logTreshold()) ret = true;
		}
	} while(false);
	//printf("%s %d \n", qPrintable(_domain), ret);
	return ret;
}

void LogDevice::setPrettyDomain(bool b)
{
	m_isPrettyDomain = b;
}

bool LogDevice::isPrettyDomain() const
{
	return m_isPrettyDomain;
}

QString LogDevice::prettyDomain(const QString &domain)
{
	if(!isPrettyDomain())
		return domain;
	static QRegExp rx("(\\\\|\\/)");
	return domain.section(rx, -1);
}

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
	FILE *f = ::fopen(qPrintable(path_to_file), "w");
	if(f) {
		m_file = f;
	}
	else {
		fprintf(stderr, "Cannot open log file '%s' for writing\n", qPrintable(path_to_file));
	}
}

void FileLogDevice::log(Log::Level level, const QMessageLogContext &context, const QString &msg)
{
	if(!m_file) return;
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
		case Log::LOG_INFO: fg = Cyan; break;
		case Log::LOG_WARN: fg = Magenta; attr = AttrBright; break;
		case Log::LOG_ERR:
		case Log::LOG_FATAL: fg = Red; attr = AttrBright; break;
		case Log::LOG_DEB:
		default: fg = White; break;
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
	QString domain = prettyDomain(domainFromContext(context));
	if(!domain.isEmpty()) {
		std::fprintf(m_file, "[%s:%d] ", qPrintable(domain), context.line);
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

static const auto KeyLevel = QStringLiteral("level");
static const auto KeyDomain = QStringLiteral("domain");
static const auto KeyMessage = QStringLiteral("message");
static const auto KeyFile = QStringLiteral("file");
static const auto KeyLine = QStringLiteral("line");
static const auto KeyFunction = QStringLiteral("function");

LogEntryMap::LogEntryMap(int level, const QString &domain, const QString &message, const QString &file, int line, const QString &function)
{
	this->operator[](KeyLevel) = level;
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
	ret += "\"level\":" + QString::number(level()) + ", ";
	ret += "\"domain\":" + domain() + ", ";
	ret += "\"message\":" + message() + ", ";
	ret += "\"file\":" + file() + ", ";
	ret += "\"line\":" + QString::number(line()) + ", ";
	ret += "\"function\":" + function() + "}";
	return ret;
}

SignalLogDevice::SignalLogDevice(QObject *parent)
	: Super(parent)
{
	setPrettyDomain(true);
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
	QString domain = prettyDomain(domainFromContext(context));
	LogEntryMap m(level, domain, msg, context.file, context.line, context.function);
	emit logEntry(m);
}
