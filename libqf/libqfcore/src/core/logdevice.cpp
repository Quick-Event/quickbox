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
/*
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
*/

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
	Q_FOREACH(auto log_device, logDevices()) {
		// check if log is enabled for this device particulary
		// checking is done twice for successful log device and once for the unsuccessful one (in isMatchingAnyDeviceLogFilter)
		if(log_device->isMatchingLogFilter(level, context.file, context.category))
			log_device->log(level, context, msg);
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
	//QString ret = QString::fromUtf8(file_name);
	static QRegExp rx("(\\\\|\\/)");
	QString ret = file_name.section(rx, -1);
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
	ret = setModulesTresholdsFromArgs(ret);
	ret = setCategoriesTresholdsFromArgs(ret);
	ret.insert(0, argv[0]);
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
	if(s_globalLogFilter.logAllCategories) {
		ret += "All defined categories (" + s_definedCategories.join(',') + ") with treshold: " + qf::core::Log::levelName(s_globalLogFilter.defaultLogTreshold);
	}
	else {
		//ret += s_globalLogFilter.inverseCategoriesFilter? "All except of ": "";
		QStringList sl;
		QMapIterator<QString, Log::Level> it(s_globalLogFilter.categoriesTresholds);
		while (it.hasNext()) {
			it.next();
			sl << it.key() + ':' + qf::core::Log::levelName(it.value());
		}
		ret += sl.join(',');
	}
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
		s_globalLogFilter.defaultLogTreshold = Log::Level::Debug;
	}
	else for(QString module : tresholds) {
		QPair<QString, Log::Level> lev = parseCategoryLevel(module);
		if(lev.first.isEmpty())
			s_globalLogFilter.defaultLogTreshold = lev.second;
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
	/*
	if(s.startsWith('^')) {
		s_globalLogFilter.inverseCategoriesFilter = true;
		s = s.mid(1);
	}
	*/
	QStringList tresholds = s.split(',', QString::SkipEmptyParts);
	setCategoriesTresholds(tresholds);
	s_globalLogFilter.logAllCategories = s_globalLogFilter.categoriesTresholds.isEmpty();
}

void LogDevice::setCategoriesTresholds(const QStringList &tresholds)
{
	for(QString category : tresholds) {
		QPair<QString, Log::Level> lev = parseCategoryLevel(category);
		category = lev.first;
		Log::Level level = lev.second;
		if(category.isEmpty()) {
			s_globalLogFilter.defaultLogTreshold = level;
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
	return s_globalLogFilter.defaultLogTreshold;
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
	if(category && category[0] && !(QLatin1String("default") == QLatin1String(category))) { // default category is implicit in QMessageLogger, so filter it out
		// category specified
		qf::core::Log::Level category_level = qf::core::Log::Level::Invalid;
		if(log_filter.logAllCategories) {
			return category_level <= log_filter.defaultLogTreshold;
		}
		/*
		else if(log_filter.inverseCategoriesFilter) {
			if(!log_filter.categoriesTresholds.contains(QLatin1String(category)))
				return category_level <= log_filter.defaultLogTreshold;
		}
		*/
		else {
			QString cat = QLatin1String(category);
			category_level = log_filter.categoriesTresholds.value(cat, qf::core::Log::Level::Invalid);
			//fprintf(stderr, "%s: '%s' -> %s\n", file_name, qPrintable(category), qPrintable(qf::core::Log::levelName(category_level)));
			return level <= category_level;
		}
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
	return level <= log_filter.defaultLogTreshold;
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
		"-v,--verbose [" + s_definedCategories.join(",][") + "]"
		"\n\t controls defined categories log verbosity"
		"\n\t\t more comma delimited categories enabled."
		"\n\t\t Case sensitive."
		"\n\t\t StartsWith or CamelCase match enabled."
		"\n\t\t Example: -v is equal to --verbose enables all categories logging"
		"\n\t\t Example: -v FO,TrM is equal to --verbose F*O*,Tr*M*";
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

bool FileLogDevice::isMatchingLogFilter(Log::Level level, const char *file_name, const char *category)
{
	bool ok = Super::isMatchingLogFilter(level, file_name, category);
	//if(QLatin1String(category) == QLatin1String("Vetra"))
	//	printf("%p %s %s:%d vs. %d -> %d\n", this, file_name, category, level, s_globalLogFilter.categoriesTresholds.value(category), ok);
	return ok;
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
		std::fprintf(m_file, "[%s:%d]", qPrintable(module), context.line);
	}
	if(context.category && context.category[0])
		std::fprintf(m_file, "(%s)", context.category);
	std::fprintf(m_file, " %s", qPrintable(msg));
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
