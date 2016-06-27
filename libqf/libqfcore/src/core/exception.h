#ifndef QF_CORE_EXCEPTION_H
#define QF_CORE_EXCEPTION_H

#include "coreglobal.h"

#include <QByteArray>
#include <QString>

#include <exception>

namespace qf {
namespace core {

#define QF_THROW(exception_type, msg) \
	do { \
		if(qf::core::Exception::isAbortOnException()) \
			qFatal("EXCEPTION %s", qPrintable(msg)); \
		else \
			throw exception_type (msg, QString("%1:%2\n%3\n").arg(__FILE__).arg(__LINE__).arg(QF_FUNC_NAME)); \
	} while(0)

//! if Exception::terminateOnException is true, abort application instead of throw. For debug purposes to get the stacktrace.
#define QF_EXCEPTION(msg) \
	QF_THROW(qf::core::Exception, msg)

//---------------------------------------------------------------------
class QFCORE_DECL_EXPORT Exception : public std::exception
{
public:
	Exception(const QString& _msg, const QString& _where = QString());
	~Exception() throw() Q_DECL_OVERRIDE {}
public:
	static constexpr bool Throw = true;
protected:
	static bool s_abortOnException;
	//QString m_type;
	QString m_msg;
	QByteArray m_what;
	QString m_where;
	QString m_stackTrace;
	//QString f_catchLocation;
protected:
	void init(const QString& _msg, const QString& _where);
public:
	virtual void log();
	//virtual QString type() const {return m_type;}
	virtual QString message() const {return m_msg;}
	virtual QString where() const {return m_where;}
	virtual QString stackTrace() const {return m_stackTrace;}
	virtual QString toString() const;
	//virtual QString catchLocation() const {return f_catchLocation;}
	//void setCatchLocation(const QString &loc) {f_catchLocation = loc;}
	//virtual const char* trace() const throw();
	const char* what() const throw() Q_DECL_OVERRIDE;
	operator const char *() const  throw(){return what();}
	// set global variables: abortOnException from command line
	// --abort-on-exception
	static void setAbortOnException(bool on) {s_abortOnException = on;}
	static bool isAbortOnException() {return s_abortOnException;}
};

}}

#endif
