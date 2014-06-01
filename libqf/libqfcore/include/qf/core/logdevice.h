#ifndef LOGDEVICE_H
#define LOGDEVICE_H

#include "coreglobal.h"

#include <QMap>

namespace qf {
namespace core {

class QFCORE_DECL_EXPORT LogDevice
{
	friend class Log;
	protected:
		///default level is LOG_ERR.
		int m_logTreshold;
	protected:
		int m_stackLevel;
		static int environmentLogTreshold;
		static int commandLineLogTreshold;
	public:
		QMap<QString, int> domainTresholds;
	public:
		int setLogTreshold(int level);
		int logTreshold();

		virtual void push() {m_stackLevel++;}
		virtual void pop() {if(m_stackLevel > 0) m_stackLevel--;}

		virtual bool checkLogPermisions(const QString &_domain, int _level);
		virtual void log(const QString &domain, int level, const QString &message, int flags = 0) = 0;
	public:
		LogDevice();
		virtual ~LogDevice() {}
};

class QFCORE_DECL_EXPORT FileLogDevice : public LogDevice
{
	public:
		static FILE *file;
		int count;
		int fgColor;
		int bgColor;
		int colorAttr;
		bool colored;
	public:
		virtual void log(const QString &domain, int level, const QString &message, int flags = 0);
		void setColorForNextLog(int fg, int bg, int attr)
		{
			fgColor = fg;
			bgColor = bg;
			colorAttr = attr;
			colored = true;
		}
	public:
		FileLogDevice() : LogDevice(), count(0), colored(false) {}
};

}
}

#endif // LOGDEVICE_H
