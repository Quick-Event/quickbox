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
public:
	static QStringList setCLITresholds(int argc, char *argv[]);

	static void install(LogDevice *dev);

	static Log::Level globalLogTreshold();

	Log::Level setLogTreshold(Log::Level level);
	Log::Level logTreshold();

	static bool checkAllLogContext(Log::Level level, const char *file_name, const char *category = nullptr);

	static void setLoggingEnabled(bool on);
	static bool isLoggingEnabled();

	void setEnabled(bool b);
	bool isEnabled() const {return m_enabled;}

	//void setPrettyDomain(bool b);
	//bool isPrettyDomain() const;

	static const char *dCommandLineSwitchHelp();

	virtual void log(Log::Level level, const QMessageLogContext &context, const QString &msg) = 0;
protected:
	static QString moduleFromFileName(const char *file_name);
	virtual bool checkLogContext(Log::Level level, const char *file_name, const char *category);
	/**
	 * @brief checkLogContext
	 * @return true if message with this context will be logged
	 */
	static bool checkGlobalLogContext(Log::Level level, const char *file_name, const char *category);
protected:
	static Log::Level s_environmentLogTreshold;
	static Log::Level s_commandLineLogTreshold;

	static QMap<QString, Log::Level> s_modulesTresholds;
	static QMap<QString, Log::Level> s_categoriesTresholds;

	static bool s_loggingEnabled;

	Log::Level m_logTreshold;
	int m_count;
	//bool m_isPrettyDomain;
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

	void log(Log::Level level, const QMessageLogContext &context, const QString &msg) Q_DECL_OVERRIDE;
protected:
	FILE *m_file;
};

class QFCORE_DECL_EXPORT LogEntryMap : public QVariantMap
{
public:
	//enum Key {KeyLevel, KeyDomain, KeyMessage, KeyFile, KeyLine, KeyFunction};
public:
	LogEntryMap() : QVariantMap() {}
	LogEntryMap(Log::Level level, const QString &domain, const QString &message, const QString &file = QString(), int line = -1, const QString &function = QString());
	LogEntryMap(const QVariantMap &m) : QVariantMap(m) {}
public:
	qf::core::Log::Level level() const;
	QString levelStr() const;
	QString domain() const;
	QString message() const;
	QString file() const;
	int line() const;
	QString function() const;
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
	~SignalLogDevice() Q_DECL_OVERRIDE;

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
