#ifndef QF_CORE_LOGDEVICE_H
#define QF_CORE_LOGDEVICE_H

#include "coreglobal.h"
#include "log.h"

#include <QMap>
#include <QObject>

namespace qf {
namespace core {

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

class QFCORE_DECL_EXPORT LogDevice : public QObject
{
	Q_OBJECT
protected:
	LogDevice(QObject *parent = 0);
public:
	virtual ~LogDevice();
public:
	virtual QString domainFromContext(const QMessageLogContext &context);
	static void install(LogDevice *dev);
	Log::Level setLogTreshold(Log::Level level);
	/// @return list of arguments wthout ones used for domain tresholds setting
	QStringList setDomainTresholds(int argc, char *argv[]);
	Log::Level logTreshold();
	virtual void log(Log::Level level, const QMessageLogContext &context, const QString &msg);
	virtual bool checkLogPermisions(const QMessageLogContext &context, Log::Level _level);

	void setPrettyDomain(bool b);
	bool isPrettyDomain() const;

	bool isEmitLogEntries() const {return m_emitLogEntries;}
	void setEmitLogEntries(bool on) {m_emitLogEntries = on;}

	Q_SIGNAL void logEntry(const LogEntryMap &log_entry_map);
protected:
	virtual QString prettyDomain(const QString &domain);
protected:
	static Log::Level environmentLogTreshold;
	static Log::Level commandLineLogTreshold;

	QMap<QString, Log::Level> m_domainTresholds;
	Log::Level m_logTreshold;
	int m_count;
	bool m_isPrettyDomain;
private:
	bool m_emitLogEntries = false;
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
/*
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

	Q_SIGNAL void logEntry(const QVariantMap &log_entry_map);
};
*/
}
}

#endif // QF_CORE_LOGDEVICE_H
