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
		level = Log::Level::Warning;
		break;
	case QtCriticalMsg:
		level = Log::Level::Error;
		break;
	case QtFatalMsg:
		level = Log::Level::Fatal;
		break;
	}
	if(level == Log::Level::Debug && msg.isEmpty()) {
		// do not log empty debug message like qDebug();
		// qfLogFuncFrame cannot be implemented optimaly without this hack
		return;
	}
	Q_FOREACH(auto log_device, logDevices()) {
		log_device->log(level, context, msg);
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
//                       LogDevice
//=========================================================
Log::Level LogDevice::s_environmentLogTreshold = environment_treshold();
Log::Level LogDevice::s_commandLineLogTreshold = Log::Level::Info;

QMap<QString, Log::Level> LogDevice::s_modulesTresholds;
QMap<QString, Log::Level> LogDevice::s_categoriesTresholds;
QStringList LogDevice::s_definedCategories;
bool LogDevice::s_logAllCategories = false;
bool LogDevice::s_inverseCategoriesFilter = false;

bool LogDevice::s_loggingEnabled = true;

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

QString LogDevice::moduleFromFileName(const char *file_name)
{
	QString ret = QString::fromUtf8(file_name);
	static QRegExp rx("(\\\\|\\/)");
	ret = ret.section(rx, -1);
	return ret;
}

void LogDevice::install(LogDevice *dev)
{
	logDevices() << dev;
}

QStringList LogDevice::setGlobalTresholds(int argc, char *argv[])
{
	QStringList ret;
	for(int i=1; i<argc; i++) {
		QString s = QString::fromUtf8(argv[i]);
		ret << s;
	}
	ret = setModulesTresholds(ret);
	ret = setDomainTresholds(ret);
	ret.insert(0, argv[0]);
	return ret;
}

QString LogDevice::modulesLogInfo()
{
	QString ret;
	QStringList sl;
	QMapIterator<QString, Log::Level> it(s_modulesTresholds);
	while (it.hasNext()) {
		it.next();
		sl << it.key() + ':' + qf::core::Log::levelName(it.value());
	}
	ret += sl.join(',');
	return ret;
}

QString LogDevice::domainsLogInfo()
{
	QString ret;
	if(s_logAllCategories) {
		ret += s_definedCategories.join(',');
	}
	else {
		ret += s_inverseCategoriesFilter? "All except of": "";
		QStringList sl;
		QMapIterator<QString, Log::Level> it(s_categoriesTresholds);
		while (it.hasNext()) {
			it.next();
			sl << it.key() + ':' + qf::core::Log::levelName(it.value());
		}
		ret += sl.join(',');
	}
	return ret;
}

QStringList LogDevice::setModulesTresholds(const QStringList &args)
{
	QStringList ret;
	s_modulesTresholds.clear();
	QStringList tresholds;
	for(int i=0; i<args.count(); i++) {
		QString s = args[i];
		if(s == QLatin1String("-d") || s == QLatin1String("--debug")) {
			i++;
			s = s.mid(2);
			tresholds << s.split(',');
		}
		else {
			ret << s;
		}
	}
	for(QString module : tresholds) {
		int ix = module.indexOf(':');
		//printf("domainTreshold %s\n", qPrintable(dom_tres));
		Log::Level level = Log::Level::Debug;
		if(ix > 0) {
			QString s = module.mid(ix + 1, 1);
			module = module.mid(0, ix);
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
		if(module.isEmpty())
			s_commandLineLogTreshold = level;
		else
			s_modulesTresholds[module] = level;
	}
	return ret;
}

QStringList LogDevice::setDomainTresholds(const QStringList &args)
{
	QStringList ret;
	s_categoriesTresholds.clear();
	QStringList tresholds;
	for(int i=0; i<args.count(); i++) {
		QString s = args[i];
		if(s == QLatin1String("-v") || s == QLatin1String("--verbose")) {
			i++;
			s = s.mid(2);
			if(s.startsWith('^')) {
				s_inverseCategoriesFilter = true;
				s = s.mid(1);
			}
			tresholds << s.split(',');
			s_logAllCategories = tresholds.isEmpty();
		}
		else {
			ret << s;
		}
	}
	for(QString domain : tresholds) {
		int ix = domain.indexOf(':');
		//printf("domainTreshold %s\n", qPrintable(dom_tres));
		Log::Level level = Log::Level::Debug;
		if(ix > 0) {
			QString s = domain.mid(ix + 1, 1);
			domain = domain.mid(0, ix);
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
		if(!domain.isEmpty())
			s_categoriesTresholds[domain] = level;
	}
	return ret;
}

Log::Level LogDevice::globalLogTreshold()
{
	if(s_commandLineLogTreshold > Log::Level::Invalid)
		return s_commandLineLogTreshold;
	if(s_environmentLogTreshold > Log::Level::Invalid)
		return s_environmentLogTreshold;
	return Log::Level::Invalid;
}

Log::Level LogDevice::logTreshold()
{
	return m_logTreshold;
}

Log::Level LogDevice::setLogTreshold(Log::Level level)
{
	Log::Level old = m_logTreshold;
	m_logTreshold = level;
	return old;
}

bool LogDevice::checkLogContext(Log::Level level, const char *file_name, const char *category)
{
	return checkGlobalLogContext(level, file_name, category);
}

bool LogDevice::checkGlobalLogContext(Log::Level level, const char *file_name, const char *category)
{
	bool ret = false;
	do {
		if(level <= Log::Level::Invalid) break;
		if(level == Log::Level::Fatal) {
			ret = true;
			break;
		}
		if(level == Log::Level::Debug) {
#ifdef QF_NO_DEBUG_LOG
			break;
#endif
		}
		if(!s_definedCategories.isEmpty() && category && category[0]) {
			// category specified
			bool category_level_found = false;
			QMapIterator<QString, Log::Level> it(s_categoriesTresholds);
			while (it.hasNext()) {
				it.next();
				if(it.key() == QLatin1String(category)) {
					//printf("found '%s' in '%s' treshold: %d \n", qPrintable(it.key()), qPrintable(_domain), it.value());
					category_level_found = true;
					if(level <= it.value())
						ret = true;
					break;
				}
			}
			if(!category_level_found) {
				if(level <= globalLogTreshold())
					ret = true;
			}
		}
		else {
			ret = true;
		}
		if(ret) {
			ret = false;
			bool module_level_found = false;
			QString module = moduleFromFileName(file_name);
			QMapIterator<QString, Log::Level> it(s_modulesTresholds);
			while (it.hasNext()) {
				it.next();
				if(module.indexOf(it.key(), 0, Qt::CaseInsensitive) >= 0) {
					//printf("found '%s' in '%s' treshold: %d \n", qPrintable(it.key()), qPrintable(_domain), it.value());
					module_level_found = true;
					if(level <= it.value())
						ret = true;
					break;
				}
			}
			if(!module_level_found) {
				if(level <= globalLogTreshold())
					ret = true;
			}
		}
	} while(false);
	//printf("%s %d \n", qPrintable(_domain), ret);
	return ret;
}

void LogDevice::setLoggingEnabled(bool on)
{
	s_loggingEnabled = on;
}

bool LogDevice::isLoggingEnabled()
{
	return s_loggingEnabled;
}

void LogDevice::setEnabled(bool b)
{
	m_enabled = b;
}

bool LogDevice::checkAllLogContext(Log::Level level, const char *file_name, const char *category)
{
	Q_FOREACH(auto log_device, logDevices()) {
		if(log_device->isEnabled() && log_device->checkLogContext(level, file_name, category)) {
			return true;
		}
	}
	return false;
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
QString LogDevice::logModulesCLIHelp()
{
	return "-d,--debug [domain[:LEVEL][,domain[:LEVEL]]]\n"
	"\t set debug domain and level\n"
	"\t\t domain: any substring of source module, for example 'mymod' prints debug info from every source file with name containing 'mymod', mymodule.cpp, tomymod.cpp, ...\n"
	"\t\t LEVEL: any of DEB, INFO, WARN, ERR, [D,I,W,E] can be used as well, default level is INFO";
}

QString LogDevice::logCategoriesCLIHelp()
{
	QString ret;
	if(s_definedCategories.isEmpty())
		return ret;
	ret =
		"-v,--verbose [^][" + s_definedCategories.join(",][") + "]"
		"\n\t controls defined categories log verbosity"
		"\n\t\t more comma delimited categories enabled."
		"\n\t\t When list starts with ^, the categories listed are excluded."
		"\n\t\t Case insensitive."
		"\n\t\t StartsWith or CamelCase match enabled."
		"\n\t\t Example: -v is equal to --verbose enables all categories logging"
		"\n\t\t Example: -v fo,tram is equal to --verbose FreeOpcua,TramMove"
		"\n\t\t Example: -v ^opcua,tram means log all except modules starting with 'opcua' or 'tram'";
	return ret;
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
	QString module = moduleFromFileName(context.file);
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
static const auto KeyCategory = QStringLiteral("category");
static const auto KeyMessage = QStringLiteral("message");
static const auto KeyFile = QStringLiteral("file");
static const auto KeyLine = QStringLiteral("line");
static const auto KeyFunction = QStringLiteral("function");

LogEntryMap::LogEntryMap(Log::Level level, const QString &category, const QString &message, const QString &file, int line, const QString &function)
{
	this->operator[](KeyLevel) = (int)level;
	this->operator[](KeyCategory) = category;
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

QString LogEntryMap::category() const
{
	return value(KeyCategory).toString();
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
	ret += "\"category\":" + category() + ", ";
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
	QString module = moduleFromFileName(context.file);
	LogEntryMap m(level, module, msg, context.file, context.line, context.function);
	emit __logEntry(m);
}
