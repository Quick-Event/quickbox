#ifndef _FLOG_H_
#define _FLOG_H_

//#include <qfstring.h>
//#include <qfstacktrace.h>

#include <QTextStream>
#include <QMapIterator>

#include <stdio.h>
#include <stdarg.h>

#include <qfcoreglobal.h>
//#include <qfcompat.h>

#if defined QT_DEBUG
	#define QFLOG_DO_TRASH_LOG
#else
	#undef QFLOG_DO_TRASH_LOG
#endif

//#define FLOG_LOG_ENTER_EXIT_FN

class QFCORE_DECL_EXPORT QFLogDevice
{
	friend class QFLog;
	protected:
	//public:
		///defaultni level je LOG_ERR.
		int fLogTreshold;
	protected:
		int stackLevel; ///< FlagPush ho zvysi a FlagPop snizi
		//int recentLogLevel;
		//QString recentDomain;
		static int environmentLogTreshold;
		static int commandLineLogTreshold;
	public:
		QMap<QString, int> domainTresholds;
	public:
		/**
		@param level nastavi jaky loglevel se jeste bude logovat, kdyz je roven 0, neloguje se,
		@return predesly log treshold
		 */
		int setLogTreshold(int level);
		int logTreshold();

		virtual void push() {stackLevel++;}
		virtual void pop() {if(stackLevel > 0) stackLevel--;}

		virtual bool checkLogPermisions(const QString &_domain, int _level);
		virtual void log(const QString &domain, int level, const QString &message, int flags = 0) = 0;
	public:
		QFLogDevice() : fLogTreshold(4), stackLevel(0) {}
		virtual ~QFLogDevice() {}
};

class QFCORE_DECL_EXPORT QFFileLogDevice : public QFLogDevice
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
		QFFileLogDevice() : QFLogDevice(), count(0), colored(false) {}
};

class QFCORE_DECL_EXPORT QFLog
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
			QFLogDevice *device;

			Stream(int _level, const QString &d = QString(), QFLogDevice *dev = NULL)
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
		QFLogDevice* device() {return stream->device;}
	protected:
		static QFLogDevice *f_defaultLogDevice;
	public:
		/// defaultLogDevice je defaultne appLogDevice
		static QFLogDevice* defaultLogDevice();
		/// kdyz budu chtit pretocit logovani uplne jinam, treba do databaze, pretocim tenhle pointer
		/// funkce vraci to, ktery to bylo dosud
		static QFLogDevice* setDefaultLogDevice(QFLogDevice *dev);
	public:
		inline QFLog& color(TerminalColor fg, TerminalColor bg = QFLog::Black, TerminalAttr a = QFLog::AttrBright)
		{
			QFFileLogDevice *dev = dynamic_cast<QFFileLogDevice*>(device());
			if(dev) dev->setColorForNextLog(fg, bg, a);
			return *this;
		}

		inline void noColor()
		{
			QFFileLogDevice *dev = dynamic_cast<QFFileLogDevice*>(device());
			if(dev) dev->colored = false;
		}

		static QString stackTrace();

		inline QFLog& push() {stream->flags = (unsigned char)FlagPush; return *this; }
		inline QFLog& pop() {stream->flags = (unsigned char)FlagPop; return *this; }
		//inline QFLog &cont() { stream->cont = true; return *this; }
		inline QFLog &doSpace() { stream->space = true; stream->ts << " "; return *this; }
		inline QFLog &noSpace() { stream->space = false; return *this; }
		inline QFLog &maybeSpace() {
			if (stream->space) stream->ts << " ";
			return *this;
		}
	public:
		int level() {return stream->level;}
		static const char* levelName(QFLog::Level level);
	public:
		inline QFLog &operator<<(Controls t) {
			switch(t) {
				case nospace: return noSpace();
				case space: return doSpace();
				default: return maybeSpace();
			}
		}
		inline QFLog &operator<<(QChar t) {if(stream->enabled) stream->ts << t; return maybeSpace(); }
		inline QFLog &operator<<(bool t) {if(stream->enabled) stream->ts << (t ? "true" : "false"); return maybeSpace(); }
		inline QFLog &operator<<(char t) {if(stream->enabled) stream->ts << t; return maybeSpace(); }
		inline QFLog &operator<<(signed short t) {if(stream->enabled) stream->ts << t; return maybeSpace(); }
		inline QFLog &operator<<(unsigned short t) {if(stream->enabled) stream->ts << t; return maybeSpace(); }
		inline QFLog &operator<<(signed int t) {if(stream->enabled) stream->ts << t; return maybeSpace(); }
		inline QFLog &operator<<(unsigned int t) {if(stream->enabled) stream->ts << t; return maybeSpace(); }
		inline QFLog &operator<<(signed long t) {if(stream->enabled) stream->ts << t; return maybeSpace(); }
		inline QFLog &operator<<(unsigned long t) {if(stream->enabled) stream->ts << t; return maybeSpace(); }
		inline QFLog &operator<<(qint64 t) {if(stream->enabled) stream->ts << QString::number(t); return maybeSpace(); }
		inline QFLog &operator<<(quint64 t) {if(stream->enabled) stream->ts << QString::number(t); return maybeSpace(); }
		inline QFLog &operator<<(float t) {if(stream->enabled) stream->ts << t; return maybeSpace(); }
		inline QFLog &operator<<(double t) {if(stream->enabled) stream->ts << t; return maybeSpace(); }
		inline QFLog &operator<<(const char* t) {if(stream->enabled) stream->ts  << t; return maybeSpace(); }
		inline QFLog &operator<<(const QString & t) {if(stream->enabled) stream->ts << t; return maybeSpace(); }
		inline QFLog &operator<<(const QLatin1String &t) {if(stream->enabled) stream->ts << t.latin1(); return maybeSpace(); }
		inline QFLog &operator<<(const QByteArray & t) {if(stream->enabled) stream->ts << t; return maybeSpace(); }
		inline QFLog &operator<<(const void * t) {if(stream->enabled) stream->ts << t; return maybeSpace(); }
		//inline QFLog &operator<<(const QVariant &v) {if(enabled) stream->ts << QFString(v); return maybeSpace(); }
		//inline QFLog &operator<<(const StackTrace &st) {Q_UNUSED(st); if(enabled) stream->ts << QFStackTrace::stackTrace(); return maybeSpace(); }

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
		inline QFLog(int level, const QString &domain = QString(), QFLogDevice *log_device = NULL)
			: stream(new Stream(level, domain, log_device))
		{
		}
		inline QFLog(const QFLog &o) : stream(o.stream) { ++stream->ref; }
		inline ~QFLog()
		{
			if(!--stream->ref) {
				if(stream->enabled) {
					if(stream->buffer.isEmpty() && (stream->flags & QFLog::FlagPop)) {
						/// prazdny message s flagem pop neloguju, protoze slouzi jen k popnuti
					}
					else {
						device()->log(stream->domain, stream->level, stream->buffer, stream->flags);
					}
					if(stream->flags & QFLog::FlagPush) device()->push();
					if(stream->flags & QFLog::FlagPop) device()->pop();
				}
				noColor();
				delete stream;
			}
		}
};

inline QFLog qfFat(const char * dom = "") {return QFLog(QFLog::LOG_FATAL, dom);}
inline QFLog qfErr(const char * dom = "") {return QFLog(QFLog::LOG_ERR, dom);}
inline QFLog qfWarn(const char * dom = "") {return QFLog(QFLog::LOG_WARN, dom);}
inline QFLog qfInf(const char * dom = "") {return QFLog(QFLog::LOG_INF, dom);}
inline QFLog qfDeb(const char * dom = "") {return QFLog(QFLog::LOG_DEB, dom);}
inline QFLog qfTrsh(const char * dom = "") {return QFLog(QFLog::LOG_TRASH, dom);}

#endif

