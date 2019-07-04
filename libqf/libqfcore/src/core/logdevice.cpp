#include "logdevice.h"
#include "utils.h"

#include <QByteArray>
#include <QString>
#include <QDateTime>

#include <stdio.h>
#include <iostream>

#ifdef Q_OS_UNIX
#include <unistd.h>
#include <cstdio>
#endif

namespace qf {
namespace core {

namespace {

QLatin1String CATEGORY_DEFAULT("default");
QLatin1String CATEGORY_QML("qml");

QList< LogDevice* >& logDevices();


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
	//if(!msg.contains("111111"))
	//	return;
	//fprintf(stderr, "%s\n", qPrintable(msg));
	Q_FOREACH(auto log_device, logDevices()) {
		// check if log is enabled for this device particulary
		// checking is done twice for successful log device and once for the unsuccessful one (in isMatchingAnyDeviceLogFilter)
		if(log_device->isMatchingLogFilter(level, context.file, context.category))
			log_device->log(level, context, msg);
	}
	if(level == Log::Level::Fatal)
		abort();
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
//                       LogDevice
//=========================================================
//Log::Level LogDevice::s_environmentLogTreshold = environment_treshold();
//Log::Level LogDevice::s_commandLineLogTreshold = Log::Level::Info;

//QMap<QString, Log::Level> LogDevice::s_modulesTresholds;
//QMap<QString, Log::Level> LogDevice::s_categoriesTresholds;
//bool LogDevice::s_logAllCategories = false;
//bool LogDevice::s_inverseCategoriesFilter = false;
LogDevice::LogFilter LogDevice::s_globalLogFilter;
QStringList LogDevice::s_definedCategories;

bool LogDevice::s_loggingEnabled = true;
bool LogDevice::s_logLongFileNames = false;

LogDevice::LogDevice(QObject *parent)
	: QObject(parent)
	//, m_logTreshold(Log::Level::Info)
	, m_count(0)
{
}

LogDevice::~LogDevice()
{
	logDevices().removeOne(this);
}

QString LogDevice::moduleFromFileName(const QString &file_name)
{
	QString ret = file_name;
	if(s_logLongFileNames)
		return ret;
	int ix = file_name.lastIndexOf('/');
#if defined Q_OS_WIN
	if(ix < 0)
		ix = file_name.lastIndexOf('\\');
#endif
	if(ix >= 0)
		ret = ret.mid(ix + 1);
	return ret;
}

void LogDevice::install(LogDevice *dev)
{
	logDevices() << dev;
}

LogDevice *LogDevice::findDevice(const QString &object_name, bool throw_exc)
{
	for(auto *ld : logDevices()) {
		if(ld->objectName() == object_name)
			return ld;
	}
	if(throw_exc)
		QF_EXCEPTION(tr("Cannot find log device by object name '%1'").arg(object_name));
	return nullptr;
}

QStringList LogDevice::setGlobalTresholds(int argc, char *argv[])
{
	QStringList args;
	for(int i=0; i<argc; i++) {
		QString s = QString::fromUtf8(argv[i]);
		args << s;
	}
	return setGlobalTresholds(args);
}

QStringList LogDevice::setGlobalTresholds(const QStringList &args)
{
	QStringList ret;
	for(int i=1; i<args.size(); i++) {
		QString s = args[i];
		if(s == "-lh" || s == "--log-help") {
			i++;
			std::cout << "log options:" << std::endl;
			std::cout << "-lh, --log-help" << std::endl;
			std::cout << "\t" << "Show logging help" << std::endl;
			std::cout << "-lfn, --log-long-file-names" << std::endl;
			std::cout << "\t" << "Log long file names" << std::endl;
			std::cout << "-d, --log-file [<pattern>]:[D|I|W|E]" << std::endl;
			std::cout << "\t" << "Set file log treshold" << std::endl;
			std::cout << "\t" << "set treshold for all files containing pattern to treshold" << std::endl;
			std::cout << "\t" << "when pattern is not set, set treshold for all files" << std::endl;
			std::cout << "\t" << "when treshold is not set, set treshold D (Debug) for all files containing pattern" << std::endl;
			std::cout << "\t" << "when nothing is not set, set treshold D (Debug) for all files" << std::endl;
			std::cout << "\t" << "Examples:" << std::endl;
			std::cout << "\t\t" << "-d" << "\t\t" << "set treshold D (Debug) for all files" << std::endl;
			std::cout << "\t\t" << "-d :W" << "\t\t" << "set treshold W (Warning) for all files" << std::endl;
			std::cout << "\t\t" << "-d foo" << "\t\t" << "set treshold D for all files containing 'foo'" << std::endl;
			std::cout << "\t\t" << "-d bar:W" << "\t" << "set treshold W (Warning) for all files containing 'bar'" << std::endl;
			std::cout << "-v, --log-category [<pattern>]:[D|I|W|E]" << std::endl;
			std::cout << "\t" << "Set category log treshold" << std::endl;
			std::cout << "\t" << "set treshold for all categories containing pattern to treshold" << std::endl;
			std::cout << "\t" << "the same rules as for module logging are applied to categiries" << std::endl;
			exit(0);
		}
		else if(s == "-lfn" || s == "--log-long-file-names") {
			i++;
			s_logLongFileNames = true;
		}
		else {
			ret << s;
		}
	}
	ret = setModulesTresholdsFromArgs(ret);
	ret = setCategoriesTresholdsFromArgs(ret);
	if(!args.isEmpty())
		ret.insert(0, args[0]);
	return ret;
}

QString LogDevice::modulesLogInfo()
{
	QString ret;
	QStringList sl;
	QMapIterator<QString, Log::Level> it(s_globalLogFilter.modulesTresholds);
	while (it.hasNext()) {
		it.next();
		sl << it.key() + ':' + qf::core::Log::levelName(it.value());
	}
	ret += sl.join(',');
	return ret;
}

QString LogDevice::categoriesLogInfo()
{
	QString ret;
	QStringList sl;
	QMapIterator<QString, Log::Level> it(s_globalLogFilter.categoriesTresholds);
	while (it.hasNext()) {
		it.next();
		sl << it.key() + ':' + qf::core::Log::levelName(it.value());
	}
	ret += sl.join(',');
	return ret;
}

bool LogDevice::isCategoryLogEnabled(const QString &category)
{
	bool ok = false;
	QMapIterator<QString, Log::Level> it(s_globalLogFilter.categoriesTresholds);
	while (it.hasNext()) {
		it.next();
		if(it.key() == category) {
			ok = (it.value() <= globalLogTreshold());
			break;
		}
	}
	return ok;
}

QPair<QString, Log::Level> LogDevice::parseCategoryLevel(const QString &category)
{
	int ix = category.indexOf(':');
	Log::Level level = Log::Level::Debug;
	QString cat = category;
	if(ix >= 0) {
		QString s = category.mid(ix + 1, 1);
		QChar l = s.isEmpty()? QChar(): s[0].toUpper();
		cat = category.mid(0, ix).trimmed();
		if(l == 'D')
			level = Log::Level::Debug;
		else if(l == 'I')
			level = Log::Level::Info;
		else if(l == 'W')
			level = Log::Level::Warning;
		else if(l == 'E')
			level = Log::Level::Error;
		else
			level = Log::Level::Invalid;
	}
	return QPair<QString, Log::Level>(cat, level);
}

QStringList LogDevice::setModulesTresholdsFromArgs(const QStringList &args)
{
	QStringList ret;
	s_globalLogFilter.modulesTresholds.clear();
	//QStringList tresholds;
	for(int i=0; i<args.count(); i++) {
		QString s = args[i];
		if(s == QLatin1String("-d") || s == QLatin1String("--debug")) {
			s = args.value(++i);
			setModulesTresholds(s);
		}
		else {
			ret << s;
		}
	}
	return ret;
}

void LogDevice::setModulesTresholds(const QString &s)
{
	QStringList tresholds = s.split(',', QString::SkipEmptyParts);
	setModulesTresholds(tresholds);
}

void LogDevice::setModulesTresholds(const QStringList &tresholds)
{
	if(tresholds.isEmpty()) {
		s_globalLogFilter.defaultModulesLogTreshold = Log::Level::Debug;
	}
	else for(QString module : tresholds) {
		QPair<QString, Log::Level> lev = parseCategoryLevel(module);
		if(lev.first.isEmpty())
			s_globalLogFilter.defaultModulesLogTreshold = lev.second;
		else
			s_globalLogFilter.modulesTresholds[lev.first] = lev.second;
	}
}

static QStringList tokenize_at_capital(const QString &category)
{
	QStringList ret;
	QString token;
	for (int i = 0; i < category.length(); ++i) {
		const QChar c = category[i];
		if(c.isUpper()) {
			if(!token.isEmpty()) {
				ret << token;
			}
			token = c;
		}
		else {
			token += c;
		}
	}
	if(!token.isEmpty()) {
		ret << token;
	}
	return ret;
}

QStringList LogDevice::setCategoriesTresholdsFromArgs(const QStringList &args)
{
	QStringList ret;
	s_globalLogFilter.categoriesTresholds.clear();
	QStringList tresholds;
	for(int i=0; i<args.count(); i++) {
		QString s = args[i];
		if(s == QLatin1String("-v") || s == QLatin1String("--verbose")) {
			s = args.value(++i);
			setCategoriesTresholds(s);
		}
		else {
			ret << s;
		}
	}
	setCategoriesTresholds(tresholds);
	return ret;
}

void LogDevice::setCategoriesTresholds(const QString &trsh)
{
	QString s = trsh;
	QStringList tresholds = s.split(',', QString::SkipEmptyParts);
	setCategoriesTresholds(tresholds);
}

void LogDevice::setCategoriesTresholds(const QStringList &tresholds)
{
	for(QString category : tresholds) {
		QPair<QString, Log::Level> lev = parseCategoryLevel(category);
		category = lev.first;
		Log::Level level = lev.second;
		if(category.isEmpty()) {
			s_globalLogFilter.defaultCategoriesLogTreshold = level;
		}
		else {
			int match_cnt = 0;
			for(const QString def_cat : definedCategories()) {
				bool match = (def_cat.compare(category, Qt::CaseInsensitive) == 0);
				if(!match) {
					//printf("\t def_cat %s\n", qPrintable(def_cat));
					QStringList def_cat_sl = tokenize_at_capital(def_cat);
					QStringList cli_cat_sl = tokenize_at_capital(category);
					for (int i = 0; i < cli_cat_sl.length(); ++i) {
						QString def_token = def_cat_sl.value(i);
						//printf("\t\t def_token %s\n", qPrintable(def_token));
						if(def_token.isEmpty()) {
							match = false;
							break;
						}
						QString cli_token = cli_cat_sl.value(i);
						//printf("\t\t cli_token %s\n", qPrintable(cli_token));
						if(!def_token.startsWith(cli_token, Qt::CaseSensitive)) {
							match = false;
							break;
						}
						match = true;
					}
				}
				if(match) {
					//printf("\t\t match with %s\n", qPrintable(def_cat));
					s_globalLogFilter.categoriesTresholds[def_cat] = level;
					match_cnt++;
				}
			}
			if(match_cnt == 0) {
				fprintf(stderr, "Category '%s' is not found in defined logging categories: %s\n", qPrintable(category), qPrintable(definedCategories().join(", ")));
			}
		}
	}
}

Log::Level LogDevice::globalLogTreshold()
{
	return s_globalLogFilter.defaultModulesLogTreshold;
}
/*
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
*/
bool LogDevice::isMatchingLogFilter(Log::Level level, const char *file_name, const char *category)
{
	return isMatchingGlobalLogFilter(level, file_name, category);
}

bool LogDevice::isMatchingGlobalLogFilter(Log::Level level, const char *file_name, const char *category)
{
	bool ret = isMatchingLogFilter(level, file_name, category, s_globalLogFilter);
	return ret;
}

bool LogDevice::isMatchingLogFilter(Log::Level level, const char *file_name, const char *category, const LogDevice::LogFilter &log_filter)
{
	if(level == Log::Level::Fatal) {
		return true;
	}
	if(level == Log::Level::Debug) {
#ifdef QF_NO_DEBUG_LOG
		return false;
#endif
	}
	//fprintf(stderr, "!!!!!!!!!!!!! %s: '%s'\n", file_name, qPrintable(category));
	if(category && category[0]
	   && !(CATEGORY_DEFAULT == QLatin1String(category))
	   && !(CATEGORY_QML == QLatin1String(category))) { // default, qml category is implicit in QMessageLogger, so filter it out
		// category specified
		qf::core::Log::Level category_level = qf::core::Log::Level::Invalid;
		QString cat = QLatin1String(category);
		category_level = log_filter.categoriesTresholds.value(cat, log_filter.defaultCategoriesLogTreshold);
		//fprintf(stderr, "%s: '%s' -> %s\n", file_name, qPrintable(category), qPrintable(qf::core::Log::levelName(category_level)));
		return level <= category_level;
	}
	{
		QString module = moduleFromFileName(file_name);
		QMapIterator<QString, Log::Level> it(log_filter.modulesTresholds);
		while (it.hasNext()) {
			it.next();
			if(module.indexOf(it.key(), 0, Qt::CaseInsensitive) >= 0) {
				//printf("found '%s' in '%s' treshold: %d \n", qPrintable(it.key()), qPrintable(_domain), it.value());
				return level <= it.value();
			}
		}
	}
	//printf("%s %d \n", qPrintable(_domain), ret);
	return level <= log_filter.defaultModulesLogTreshold;
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

bool LogDevice::isMatchingAnyDeviceLogFilter(Log::Level level, const char *file_name, const char *category)
{
	Q_FOREACH(auto log_device, logDevices()) {
		if(log_device->isEnabled()) {
			bool ok = log_device->isMatchingLogFilter(level, file_name, category);
			//if(category && category[0])
			//	printf("%s %s %d\n", log_device->metaObject()->className(), category, ok);
			if(ok)
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
	return
	"-lfn, --log-long-file-names"
	"\n\t log long file names"
	"\n-d,--debug [PATTERN[:TRESHOLD][,PATTERN[:TRESHOLD]]]"
	"\n\t set file log pattern and treshold"
	"\n\t\t PATTERN: any substring of source module, for example 'mymod' prints debug info from every source file with name containing 'mymod', mymodule.cpp, tomymod.cpp, ..."
	"\n\t\t TRESHOLD: any of DEB, INFO, WARN, ERR, [D,I,W,E] can be used as well, default level is INFO"
	"\n\t when pattern is not set, set treshold for all files"
	"\n\t when treshold is not set, set treshold D (Debug) for all files containing pattern"
	"\n\t when nothing is not set, set treshold D (Debug) for all files"
	"\n\t Examples:"
	"\n\t\t-d\t\tset treshold D (Debug) for all files"
	"\n\t\t-d :W\t\tset treshold W (Warning) for all files"
	"\n\t\t-d foo\t\tset treshold D for all files containing 'foo'"
	"\n\t\t-d bar:W\tset treshold W (Warning) for all files containing 'bar'";
}

QString LogDevice::logCategoriesCLIHelp()
{
	QString ret;
	if(s_definedCategories.isEmpty())
		return ret;
	ret =
		"-v,--verbose [" + s_definedCategories.join(",][") + "]"
		"\n\t controls defined categories log verbosity"
		"\n\t\t more comma delimited categories enabled."
		"\n\t\t Case sensitive."
		"\n\t\t CamelCase match supported -v FB,TrM is equal to -v FooBar,TreeMark"
		"\n\tExamples:"
		"\n\t\t-v enables all categories logging with DEBUG treshold"
		"\n\t\t-v :W set log treshold WARNING to all categories"
		"\n\t\t-v Cat1:W,Cat2:I set log treshold WARNING to categorie Cat1 and INFO to Cat2";
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

void FileLogDevice::setFile(const QString &path_to_file, bool append)
{
	if(m_file && m_file != stderr) {
		::fclose(m_file);
	}
	if(path_to_file.isEmpty()) {
		m_file = stderr;
	}
	else {
		const char *mode = append? "a": "w";
		FILE *f = ::fopen(qPrintable(path_to_file), mode);
		if(f) {
			std::fprintf(stderr, "Redirecting log to file: %s\n", qPrintable(path_to_file));
			m_file = f;
		}
		else {
			std::fprintf(stderr, "Cannot open log file '%s' for writing\n", qPrintable(path_to_file));
		}
	}
}

bool FileLogDevice::isMatchingLogFilter(Log::Level level, const char *file_name, const char *category)
{
	bool ok = Super::isMatchingLogFilter(level, file_name, category);
	//if(QLatin1String(category) == QLatin1String("Vetra"))
	//	printf("%p %s %s:%d vs. %d -> %d\n", this, file_name, category, level, s_globalLogFilter.categoriesTresholds.value(category), ok);
	return ok;
}

namespace {

enum TTYColor {Black=0, Red, Green, Yellow, Blue, Magenta, Cyan, White};

void set_TTY_color(FILE *file, TTYColor color, bool bright)
{
	std::fprintf(file, "\033[");
	std::putc(bright? '1': '0', file);
	std::putc(';', file);
	std::putc('3', file);
	std::putc('0' + color, file);
	std::putc('m', file);
}

}

void FileLogDevice::log(Log::Level level, const QMessageLogContext &context, const QString &msg)
{
	if(!m_file)
		return;

#ifndef Q_OS_WIN
	bool is_TTI = (m_file == stderr && ::isatty(STDERR_FILENO));
#else
	bool is_TTI = false;
#endif

	QDateTime dt = QDateTime::currentDateTime();
	QString s = dt.toString(Qt::ISODate);
	QByteArray ba = s.toLatin1();
	if(is_TTI) set_TTY_color(m_file, TTYColor::Green, false);
	std::fprintf(m_file, " %s", ba.constData());
	std::fprintf(m_file, ".%03d", dt.time().msec());

	if(is_TTI) set_TTY_color(m_file, TTYColor::Yellow, false);
	std::fprintf(m_file, "[%s:%d]", qPrintable(moduleFromFileName(context.file)), context.line);

	if(context.category && context.category[0] && context.category != QLatin1String("default")) {
		if(is_TTI) set_TTY_color(m_file, TTYColor::White, true);
		std::fprintf(m_file, "(%s)", context.category);
	}
	switch(level) {
	case Log::Level::Fatal:
		if(is_TTI) set_TTY_color(m_file, TTYColor::Red, true);
		std::fprintf(m_file, "|F|");
		break;
	case Log::Level::Error:
		if(is_TTI) set_TTY_color(m_file, TTYColor::Red, true);
		std::fprintf(m_file, "|E|");
		break;
	case Log::Level::Warning:
		if(is_TTI) set_TTY_color(m_file, TTYColor::Magenta, true);
		std::fprintf(m_file, "|W|");
		break;
	case Log::Level::Info:
		if(is_TTI) set_TTY_color(m_file, TTYColor::Cyan, true);
		std::fprintf(m_file, "|I|");
		break;
	case Log::Level::Debug:
		if(is_TTI) set_TTY_color(m_file, TTYColor::White, false);
		std::fprintf(m_file, "|D|");
		break;
	default:
		if(is_TTI) set_TTY_color(m_file, TTYColor::Red, true);
		std::fprintf(m_file, "|?|");
		break;
	};
	std::putc(' ', m_file);

	std::fprintf(m_file, " %s", qPrintable(msg));

	if(is_TTI)
		std::fprintf(m_file, "\33[0m");
#ifdef Q_OS_WIN
	std::fprintf(m_file, "\r\n");
#else
	std::fprintf(m_file, "\n");
#endif
	std::fflush(m_file);
}

//=========================================================
// LogEntryMap
//=========================================================
static const auto KEY_LEVEL = QStringLiteral("level");
static const auto KEY_CATEGORY = QStringLiteral("category");
static const auto KEY_MESSAGE = QStringLiteral("message");
static const auto KEY_FILE = QStringLiteral("file");
static const auto KEY_LINE = QStringLiteral("line");
static const auto KEY_FUNCTION = QStringLiteral("function");
static const auto KEY_TIME_STAMP = QStringLiteral("timestamp");

LogEntryMap::LogEntryMap(Log::Level level, const QString &category, const QString &message, const QString &file, int line, const QString &function)
{
	this->operator[](KEY_LEVEL) = (int)level;
	this->operator[](KEY_CATEGORY) = category;
	this->operator[](KEY_MESSAGE) = message;
	this->operator[](KEY_FILE) = file;
	this->operator[](KEY_LINE) = line;
	this->operator[](KEY_FUNCTION) = function;
	this->operator[](KEY_TIME_STAMP) = QDateTime::currentDateTime();
}

Log::Level LogEntryMap::level() const
{
	return (Log::Level)value(KEY_LEVEL).toInt();
}

LogEntryMap &LogEntryMap::setLevel(Log::Level l)
{
	(*this)[KEY_LEVEL] = (int)l;
	return *this;
}

QString LogEntryMap::levelStr() const
{
	return Log::levelName(level());
}

QString LogEntryMap::message() const
{
	return value(KEY_MESSAGE).toString();
}

LogEntryMap &LogEntryMap::setMessage(const QString &m)
{
	(*this)[KEY_MESSAGE] = m;
	return *this;
}

QString LogEntryMap::category() const
{
	return value(KEY_CATEGORY).toString();
}

LogEntryMap &LogEntryMap::setCategory(const QString &c)
{
	(*this)[KEY_CATEGORY] = c;
	return *this;
}

QString LogEntryMap::file() const
{
	return value(KEY_FILE).toString();
}

LogEntryMap &LogEntryMap::setFile(const QString &f)
{
	(*this)[KEY_FILE] = f;
	return *this;
}

int LogEntryMap::line() const
{
	return value(KEY_LINE).toInt();
}

LogEntryMap &LogEntryMap::setLine(int l)
{
	(*this)[KEY_LINE] = l;
	return *this;
}

QString LogEntryMap::function() const
{
	return value(KEY_FUNCTION).toString();
}

LogEntryMap &LogEntryMap::setFunction(const QString &f)
{
	(*this)[KEY_FUNCTION] = f;
	return *this;
}

QDateTime LogEntryMap::timeStamp() const
{
	return value(KEY_TIME_STAMP).toDateTime();
}

LogEntryMap &LogEntryMap::setTimeStamp(const QDateTime &ts)
{
	(*this)[KEY_TIME_STAMP] = ts;
	return *this;
}

QString LogEntryMap::toString() const
{
	QString ret = "{";
	ret += "\"level\":" + QString::number((int)level()) + ", ";
	ret += "\"category\":\"" + category() + "\", ";
	ret += "\"message\":\"" + message() + "\", ";
	ret += "\"file\":\"" + file() + "\", ";
	ret += "\"line\":" + QString::number(line()) + ", ";
	ret += "\"time\":\"" + timeStamp().toString(Qt::ISODate) + "\", ";
	ret += "\"function\":\"" + function() + "\"}";
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

SignalLogDevice *SignalLogDevice::install()
{
	SignalLogDevice *ret = new SignalLogDevice();
	Super::install(ret);
	return ret;
}

void SignalLogDevice::log(Log::Level level, const QMessageLogContext &context, const QString &msg)
{
	LogEntryMap m(level, context.category, msg, context.file, context.line, context.function);
	emit __logEntry(m);
}

}}
