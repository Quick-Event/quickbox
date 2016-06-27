#ifndef QF_CORE_LOGDEVICE_H
#define QF_CORE_LOGDEVICE_H

#include "coreglobal.h"
#include "logcore.h"

#include <QVariantMap>
#include <QObject>

namespace qf {
namespace core {

class QFCORE_DECL_EXPORT LogDevice : public QObject
{
	Q_OBJECT
protected:
	LogDevice(QObject *parent = 0);
public:
	virtual ~LogDevice();

	static void install(LogDevice *dev);
	static LogDevice* findDevice(const QString &object_name, bool throw_exc = true);

	static QStringList setGlobalTresholds(int argc, char *argv[]);
	static void setModulesTresholds(const QString &s);
	static void setCategoriesTresholds(const QString &s);
	static QString modulesLogInfo();
	static QString categoriesLogInfo();

	static QPair<QString, Log::Level> parseCategoryLevel(const QString &category);

	static void setDefinedCategories(const QStringList &categories) {s_definedCategories = categories;}
	static QStringList definedCategories() { return s_definedCategories; }
	static bool isCategoryLogEnabled(const QString &category);

	static Log::Level globalLogTreshold();

	//Log::Level setLogTreshold(Log::Level level);
	//Log::Level logTreshold();

	static bool isMatchingAnyDeviceLogFilter(Log::Level level, const char *file_name, const char *category = nullptr);

	virtual bool isMatchingLogFilter(Log::Level level, const char *file_name, const char *category);

	static void setLoggingEnabled(bool on);
	static bool isLoggingEnabled();

	void setEnabled(bool b);
	bool isEnabled() const {return m_enabled;}

	static QString logModulesCLIHelp();
	static QString logCategoriesCLIHelp();

	static QString moduleFromFileName(const QString &file_name);

	virtual void log(Log::Level level, const QMessageLogContext &context, const QString &msg) = 0;
protected:
	struct QFCORE_DECL_EXPORT LogFilter
	{
		Log::Level defaultLogTreshold = qf::core::Log::Level::Info;
		QMap<QString, Log::Level> modulesTresholds;
		QMap<QString, Log::Level> categoriesTresholds;
		bool logAllCategories = false;
		//bool inverseCategoriesFilter = false;
	};
	static bool isMatchingLogFilter(Log::Level level, const char *file_name, const char *category, const LogFilter &log_filter);
	static bool isMatchingGlobalLogFilter(Log::Level level, const char *file_name, const char *category);

	static QStringList setModulesTresholdsFromArgs(const QStringList &args);
	static void setModulesTresholds(const QStringList &tresholds);
	static QStringList setCategoriesTresholdsFromArgs(const QStringList &args);
	static void setCategoriesTresholds(const QStringList &tresholds);
protected:
	static QStringList s_definedCategories;
	static bool s_loggingEnabled;
	static LogFilter s_globalLogFilter;

	int m_count;
	bool m_enabled = true;
};

class QFCORE_DECL_EXPORT FileLogDevice : public LogDevice
{
	Q_OBJECT
private:
	typedef LogDevice Super;
protected:
	FileLogDevice(QObject *parent = 0);
public:
	~FileLogDevice() Q_DECL_OVERRIDE;
	static FileLogDevice* install();

	void setFile(const QString &path_to_file);

	bool isMatchingLogFilter(Log::Level level, const char *file_name, const char *category) Q_DECL_OVERRIDE;
	void log(Log::Level level, const QMessageLogContext &context, const QString &msg) Q_DECL_OVERRIDE;
protected:
	FILE *m_file;
};

class QFCORE_DECL_EXPORT LogEntryMap : public QVariantMap
{
public:
	LogEntryMap() : QVariantMap() {}
	LogEntryMap(Log::Level level, const QString &category, const QString &message, const QString &file = QString(), int line = -1, const QString &function = QString());
	LogEntryMap(const QVariantMap &m) : QVariantMap(m) {}
public:
	qf::core::Log::Level level() const;
	LogEntryMap& setLevel(qf::core::Log::Level l);
	QString levelStr() const;
	QString category() const;
	LogEntryMap& setCategory(const QString &c);
	QString message() const;
	LogEntryMap& setMessage(const QString &m);
	QString file() const;
	LogEntryMap& setFile(const QString &f);
	int line() const;
	LogEntryMap& setLine(int l);
	QString function() const;
	LogEntryMap& setFunction(const QString &f);
	QDateTime timeStamp() const;
	LogEntryMap& setTimeStamp(const QDateTime &ts);

	virtual QString toString() const;
};

class QFCORE_DECL_EXPORT SignalLogDevice : public LogDevice
{
	Q_OBJECT
private:
	typedef LogDevice Super;
protected:
	SignalLogDevice(QObject *parent = 0);
public:
	//~SignalLogDevice() Q_DECL_OVERRIDE;

	static SignalLogDevice* install();

	void log(Log::Level level, const QMessageLogContext &context, const QString &msg) Q_DECL_OVERRIDE;

	Q_SIGNAL void logEntry(const LogEntryMap &log_entry_map);
private:
	Q_SIGNAL void __logEntry(const LogEntryMap &log_entry_map);
};

}
}

// signal logEntry() must not be used by ???queued??? (why?, direct connection should IMO stay here) connection to avoid recursive logging
// finaly I've found that qMessageHandler itself is recursion safe
Q_DECLARE_METATYPE(qf::core::LogEntryMap)

#endif // QF_CORE_LOGDEVICE_H
