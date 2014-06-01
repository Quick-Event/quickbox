#ifndef QF_CORE_LOG_H
#define QF_CORE_LOG_H

#include "coreglobal.h"
#include "logdevice.h"

#include <QTextStream>
#include <QMapIterator>

#include <stdio.h>
#include <stdarg.h>

#if defined QT_DEBUG
	#define Log_DO_TRASH_LOG
#else
	#undef Log_DO_TRASH_LOG
#endif

#define QF_FUNC_NAME Q_FUNC_INFO

namespace qf {
namespace core {

class QFCORE_DECL_EXPORT Log
{
	public:
		enum Flag {FlagPush = 1, FlagPop = 2};
		enum Level {LOG_NULL = 0, LOG_FATAL, LOG_ERR, LOG_WARN, LOG_INF, LOG_DEB, LOG_TRASH};
		enum TerminalColor {Black = 0, Red, Green, Yellow, Blue, Magenta, Cyan, White};
		enum TerminalAttr {AttrReset = 0, AttrBright, AttrDim, AttrUnderline, AttrBlink, AttrReverse = 7, AttrHidden};
		enum Controls {nospace = 0, space, maybespace};
	protected:
		static const char *levelNames[];
		struct Stream {
			QTextStream ts;
			QString buffer;
			int ref; ///< loglevel of this instance of log object
			int level;
			QString domain; ///< log domain of this instance of log object
			bool space;
			bool enabled;
			unsigned char flags;
			LogDevice *device;

			Stream(int _level, const QString &d = QString(), LogDevice *dev = NULL)
				: ts(&buffer, QIODevice::WriteOnly), ref(1), level(_level), domain(d), space(true), flags(0)
			{
				if(dev == NULL) device = defaultLogDevice();
				else device = dev;
				enabled = false;
				if(device) {
					enabled = device->checkLogPermisions(domain, level);
				}
			}
		} *stream;
		LogDevice* device() {return stream->device;}
	protected:
		static LogDevice *f_defaultLogDevice;
	public:
		/// defaultLogDevice je defaultne appLogDevice
		static LogDevice* defaultLogDevice();
		/// kdyz budu chtit pretocit logovani uplne jinam, treba do databaze, pretocim tenhle pointer
		/// funkce vraci to, ktery to bylo dosud
		static LogDevice* setDefaultLogDevice(LogDevice *dev);
	public:
		inline Log& color(TerminalColor fg, TerminalColor bg = Log::Black, TerminalAttr a = Log::AttrBright)
		{
			FileLogDevice *dev = dynamic_cast<FileLogDevice*>(device());
			if(dev) dev->setColorForNextLog(fg, bg, a);
			return *this;
		}

		inline void noColor()
		{
			FileLogDevice *dev = dynamic_cast<FileLogDevice*>(device());
			if(dev) dev->colored = false;
		}

		static QString stackTrace();

		inline Log& push() {stream->flags = (unsigned char)FlagPush; return *this; }
		inline Log& pop() {stream->flags = (unsigned char)FlagPop; return *this; }
		//inline Log &cont() { stream->cont = true; return *this; }
		inline Log &doSpace() { stream->space = true; stream->ts << " "; return *this; }
		inline Log &noSpace() { stream->space = false; return *this; }
		inline Log &maybeSpace() {
			if (stream->space) stream->ts << " ";
			return *this;
		}
	public:
		int level() {return stream->level;}
		static const char* levelName(Log::Level level);
	public:
		inline Log &operator<<(Controls t) {
			switch(t) {
				case nospace: return noSpace();
				case space: return doSpace();
				default: return maybeSpace();
			}
		}
		inline Log &operator<<(QChar t) {if(stream->enabled) stream->ts << t; return maybeSpace(); }
		inline Log &operator<<(bool t) {if(stream->enabled) stream->ts << (t ? "true" : "false"); return maybeSpace(); }
		inline Log &operator<<(char t) {if(stream->enabled) stream->ts << t; return maybeSpace(); }
		inline Log &operator<<(signed short t) {if(stream->enabled) stream->ts << t; return maybeSpace(); }
		inline Log &operator<<(unsigned short t) {if(stream->enabled) stream->ts << t; return maybeSpace(); }
		inline Log &operator<<(signed int t) {if(stream->enabled) stream->ts << t; return maybeSpace(); }
		inline Log &operator<<(unsigned int t) {if(stream->enabled) stream->ts << t; return maybeSpace(); }
		inline Log &operator<<(signed long t) {if(stream->enabled) stream->ts << t; return maybeSpace(); }
		inline Log &operator<<(unsigned long t) {if(stream->enabled) stream->ts << t; return maybeSpace(); }
		inline Log &operator<<(qint64 t) {if(stream->enabled) stream->ts << QString::number(t); return maybeSpace(); }
		inline Log &operator<<(quint64 t) {if(stream->enabled) stream->ts << QString::number(t); return maybeSpace(); }
		inline Log &operator<<(float t) {if(stream->enabled) stream->ts << t; return maybeSpace(); }
		inline Log &operator<<(double t) {if(stream->enabled) stream->ts << t; return maybeSpace(); }
		inline Log &operator<<(const char* t) {if(stream->enabled) stream->ts  << t; return maybeSpace(); }
		inline Log &operator<<(const QString & t) {if(stream->enabled) stream->ts << t; return maybeSpace(); }
		inline Log &operator<<(const QLatin1String &t) {if(stream->enabled) stream->ts << t.latin1(); return maybeSpace(); }
		inline Log &operator<<(const QByteArray & t) {if(stream->enabled) stream->ts << t; return maybeSpace(); }
		inline Log &operator<<(const void * t) {if(stream->enabled) stream->ts << t; return maybeSpace(); }
		//inline Log &operator<<(const QVariant &v) {if(enabled) stream->ts << QFString(v); return maybeSpace(); }
		//inline Log &operator<<(const StackTrace &st) {Q_UNUSED(st); if(enabled) stream->ts << QFStackTrace::stackTrace(); return maybeSpace(); }

		static void setDefaultLogTreshold(int level)
		{
			if(defaultLogDevice()) defaultLogDevice()->setLogTreshold(level);
		}
		static int defaultLogTreshold()
		{
			if(defaultLogDevice()) return defaultLogDevice()->logTreshold();
			return LOG_NULL;
		}

		//! sets domain dresholds from command line arguments
		//! -ddomain_name=(TRASH | DEB | INF | WARN | ERR)
		//! log levels are case insensitive, if none is specified (-ddomain_name) TRASH is used.
		//! Returns list of unused arguments.
		static QStringList setDomainTresholds(int argc, char *argv[]);
		static void setDomainTresholds(const QStringList &dom_tres_list);
		/**
		 * @param new_log_stream pointer to the new log stream,
		 * if NULL, default logging to stderr is set
		 */
		static void redirectDefaultLogFile(FILE *new_log_stream = NULL);
	public:
		inline Log(int level, const QString &domain = QString(), LogDevice *log_device = NULL)
			: stream(new Stream(level, domain, log_device))
		{
		}
		inline Log(const Log &o) : stream(o.stream) { ++stream->ref; }
		inline ~Log()
		{
			if(!--stream->ref) {
				if(stream->enabled) {
					if(stream->buffer.isEmpty() && (stream->flags & Log::FlagPop)) {
						/// prazdny message s flagem pop neloguju, protoze slouzi jen k popnuti
					}
					else {
						device()->log(stream->domain, stream->level, stream->buffer, stream->flags);
					}
					if(stream->flags & Log::FlagPush) device()->push();
					if(stream->flags & Log::FlagPop) device()->pop();
				}
				noColor();
				delete stream;
			}
		}
};

}
}
/*
inline qf::core::Log qfFat(const char * dom = "") {return qf::core::Log(qf::core::Log::LOG_FATAL, dom);}
inline qf::core::Log qfErr(const char * dom = "") {return qf::core::Log(qf::core::Log::LOG_ERR, dom);}
inline qf::core::Log qfWarn(const char * dom = "") {return qf::core::Log(qf::core::Log::LOG_WARN, dom);}
inline qf::core::Log qfInf(const char * dom = "") {return qf::core::Log(qf::core::Log::LOG_INF, dom);}
inline qf::core::Log qfDeb(const char * dom = "") {return qf::core::Log(qf::core::Log::LOG_DEB, dom);}
inline qf::core::Log qfTrsh(const char * dom = "") {return qf::core::Log(qf::core::Log::LOG_TRASH, dom);}
*/
#endif

