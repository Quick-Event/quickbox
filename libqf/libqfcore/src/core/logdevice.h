#ifndef LOGDEVICE_H
#define LOGDEVICE_H

#include "coreglobal.h"
#include "log.h"

#include <QMap>
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
	static QString domainFromContext(const QMessageLogContext &context);
	static void install(LogDevice *dev);
	Log::Level setLogTreshold(Log::Level level);
	void setDomainTresholds(int argc, char *argv[]);
	Log::Level logTreshold();
	virtual void log(Log::Level level, const QMessageLogContext &context, const QString &msg) = 0;
	virtual bool checkLogPermisions(const QString &_domain, Log::Level _level);

	void setPrettyDomain(bool b);
	bool isPrettyDomain() const;
protected:
	virtual QString prettyDomain(const QString &domain);
protected:
	static Log::Level environmentLogTreshold;
	static Log::Level commandLineLogTreshold;

	QMap<QString, int> m_domainTresholds;
	Log::Level m_logTreshold;
	int m_count;
	bool m_isPrettyDomain;
};

class QFCORE_DECL_EXPORT FileLogDevice : public LogDevice
{
	Q_OBJECT
private:
	typedef LogDevice Super;
protected:
	FileLogDevice(QObject *parent = 0);
	~FileLogDevice() Q_DECL_OVERRIDE;
public:
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
	LogEntryMap(int level, const QString &domain, const QString &message, const QString &file = QString(), int line = -1, const QString &function = QString());
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
	~SignalLogDevice() Q_DECL_OVERRIDE;
public:
	static SignalLogDevice* install();

	void log(Log::Level level, const QMessageLogContext &context, const QString &msg) Q_DECL_OVERRIDE;

	Q_SIGNAL void logEntry(const QVariantMap &log_entry_map);
};

}
}

#endif // LOGDEVICE_H
