#ifndef _QFEXCEPTION_H_
#define _QFEXCEPTION_H_

#include <qflog.h>
//#include <qfstring.h>
#include <qfcoreglobal.h>
//#include <qfcompat.h>

//#include <QString>

#include <exception>

#define QF_THROW(exception_type, msg, abort) \
	do { \
		if(abort) \
			QFLog(QFLog::LOG_FATAL, "EXCEPTION") << QString("%1:%2 %3").arg(__FILE__).arg(__LINE__).arg(QF_FUNC_NAME) << msg; \
		else \
			throw exception_type (msg, QString("%1:%2\n%3\n").arg(__FILE__).arg(__LINE__).arg(QF_FUNC_NAME)); \
	} while(0)

//#define QF_ABORT_EXCEPTION(msg)
//	QF_THROW(QFException, msg, QFException::exceptionAbortsApplication)

//! if QFException::exceptionAbortsApplication is true, abort application instead of throw. For debug purposes to get the stacktrace.
#define QF_EXCEPTION(msg) \
	QF_THROW(QFException, msg, QFException::isExceptionAbortsApplication())
	//throw QFException(msg, QString("%1:%2\n%3\n").arg(__FILE__).arg(__LINE__).arg(QF_FUNC_NAME));
#define QF_SQL_EXCEPTION(msg) \
	QF_THROW(QFSqlException, msg, QFException::isExceptionAbortsApplication())
	//throw QFSqlException(msg, QString("%1:%2\n%3\n").arg(__FILE__).arg(__LINE__).arg(QF_FUNC_NAME));
/// je to spis assert
#define QF_INTERNAL_ERROR(msg)  \
	do { \
		if(QFException::isAssertThrowsException()) { \
			throw QFInternalErrorException(msg, QString("%1:%2 %3").arg(__FILE__).arg(__LINE__).arg(QF_FUNC_NAME)); \
		} \
		else { \
			QFLog(QFLog::LOG_FATAL, "QF_INTERNAL_ERROR_EXCEPTION") << QString("%1:%2 %3").arg(__FILE__).arg(__LINE__).arg(QF_FUNC_NAME) <<  msg; \
		} \
	} while(0)

//	QF_THROW(QFInternalErrorException, msg, QFException::exceptionAbortsApplication)
	//throw QFInternalErrorException(msg, QString("%1:%2\n%3\n").arg(__FILE__).arg(__LINE__).arg(QF_FUNC_NAME));
//---------------------------------------------------------------------
class QFCORE_DECL_EXPORT QFException : public std::exception
{
	protected:
		QString f_type;
		QString fmsg;
		QString fwhere;
		QString fStackTrace;
		QString f_catchLocation; ///< Sem se muze napsat, kde byl catch, ktery to odchytil.
		QByteArray qstring2cstring_whatBuffer;
	protected:
		static bool f_exceptionAbortsApplication;
		static bool f_assertThrowsException;
		static bool f_logStackTrace;
	protected:
		void init(const QString& type, const QString& _msg, const QString& _where);
		void log(int level);
		static QFException& recentExceptionRef();
	public:
		virtual void log();
		virtual QString type() const {return f_type;}
		virtual QString msg() const {return fmsg;}
		virtual QString where() const {return fwhere;}
		virtual QString stackTrace() const {return fStackTrace;}
		virtual QString catchLocation() const {return f_catchLocation;}
		void setCatchLocation(const QString &loc) {f_catchLocation = loc;}
		//virtual const char* trace() const throw();
		virtual const char* what() const throw();
		operator const char *() const  throw(){return what();}

	//! if QFException::exceptionAbortsApplication is true, QF_EXCEPTION() aborts application instead of throwing an exception.
	//! For debug purposes to get the stactrace.
		static bool isExceptionAbortsApplication();
		static bool isAssertThrowsException();
		static bool isLogStackTrace();
		static void setExceptionAbortsApplication(bool b);
		static void setAssertThrowsException(bool b);
		static void setLogStackTrace(bool b);
		
	/// nastavi globalni promenne exceptionAbortsApplication, assertThrowsException, logStackTrace z parametru prikazove radky
	/// --exception-aborts, --assert-throws, --log-stacktrace
		static void setGlobalFlags(int argc, char *argv[]);

		static const QFException& recentException() {return recentExceptionRef();}
	public:
		QFException()
		{
			init("QFException", QString(), QString());
		}
		/*
		QFException(const QString& _type, const QString& _msg, const QString& _where)
		{
			init(_type, _msg, _where);
			log();
		}
		*/
		QFException(const QString& _msg, const QString& _where = QString())
		{
			init("QFException", _msg, _where);
			log();
		}
		~QFException() throw() {}
};

class QFCORE_DECL_EXPORT QFInternalErrorException : public QFException
{
	protected:
		void init(const QString& _msg, const QString& _where);
		void log();
	public:
		QFInternalErrorException(const QString& _msg, const QString& _where = QString())
	: QFException()
		{
			init(_msg, _where);
			log();
		}
};

class QFCORE_DECL_EXPORT QFSqlException : public QFException
{
	protected:
		void log();
	public:
		QFSqlException(const QString& _msg = QString(), const QString& _where = QString())
	: QFException()
		{
			init("QFSqlException", _msg, _where);
			//log();
		}
};


//typedef QFLib::Exception QFException;

#endif
