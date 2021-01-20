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
			qf::core::Exception::fatalMessage(QString("%4 at n%1:%2 %3").arg(__FILE__).arg(__LINE__).arg(QF_FUNC_NAME).arg(msg)); \
		else \
			throw exception_type (msg, QString("%1:%2 at %3").arg(__FILE__).arg(__LINE__).arg(QF_FUNC_NAME)); \
	} while(0)

#define QF_EXCEPTION(msg) \
	QF_THROW(qf::core::Exception, msg)

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
protected:
	void init(const QString& _msg, const QString& _where);
public:
	void log();
	virtual QString message() const {return m_msg;}
	virtual QString where() const {return m_where;}
	virtual QString stackTrace() const {return m_stackTrace;}
	virtual QString toString() const;
	const char* what() const throw() Q_DECL_OVERRIDE;
	operator const char *() const  throw(){return what();}

	// set global variables: abortOnException from command line
	// --abort-on-exception
	static void setAbortOnException(bool on);
	static bool isAbortOnException();
	static void fatalMessage(const QString &msg);
};

}}

#endif
