#ifndef LOGDEVICE_H
#define LOGDEVICE_H

#include "coreglobal.h"
#include "log.h"

#include <QMap>

namespace qf {
namespace core {

class QFCORE_DECL_EXPORT LogDevice
{
	protected:
		LogDevice(int argc, char *argv[]);
	public:
		virtual ~LogDevice();
	public:
		static QString domainFromContext(const QMessageLogContext &context);
		static void install(LogDevice *dev);
		int setLogTreshold(Log::Level level);
		void setDomainTresholds(int argc, char *argv[]);
		int logTreshold();
		virtual void log(Log::Level level, const QMessageLogContext &context, const QString &msg) = 0;
		virtual bool checkLogPermisions(const QString &_domain, int _level);

		void setPrettyDomain(bool b);
		bool isPrettyDomain() const;
	protected:
		virtual QString prettyDomain(const QString &domain);
	protected:
		static int environmentLogTreshold;
		static int commandLineLogTreshold;

		QMap<QString, int> m_domainTresholds;
		Log::Level m_logTreshold;
		int m_count;
		bool m_isPrettyDomain;
};

class QFCORE_DECL_EXPORT FileLogDevice : public LogDevice
{
	private:
	typedef LogDevice Super;
	protected:
		FileLogDevice(int argc, char *argv[]);
		~FileLogDevice() Q_DECL_OVERRIDE;
	public:
		static FileLogDevice* install(int argc, char *argv[]);

		void setFile(const QString &path_to_file);

		void log(Log::Level level, const QMessageLogContext &context, const QString &msg) Q_DECL_OVERRIDE;
	protected:
		FILE *m_file;
};

}
}

#endif // LOGDEVICE_H
