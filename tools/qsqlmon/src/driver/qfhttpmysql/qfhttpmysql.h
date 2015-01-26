
//
// Author: Frantisek Vacek <fanda.vacek@volny.cz>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef QFHTTPMYSQL_H
#define QFHTTPMYSQL_H

//#include <qfexception.h>
#include "../../qfclassfield.h"

#include <QSqlDriver>
#include <QSqlResult>
#include <QSqlRecord>
//#include <QNetworkReply>

class QBuffer;
class QHttp;
class QEventLoop;
class QTimer;
class QBuffer;
class QUrl;
class QFHttpMySqlDriver;
class QNetworkAccessManager;
class QNetworkRequest;
class QNetworkReply;

class QFHttpMySqlDriverHttp : public QObject
{
	Q_OBJECT
	QF_FIELD_RW(QString, h, setH, ost)
	QF_FIELD_RW(int, p, setP, ort)
	protected:
		QNetworkAccessManager *f_http;
		//QEventLoop *f_eventLoop;
		QTimer *f_timeoutTimer;
		QString f_lastError;
		//QNetworkReply *f_currentReply;
		//int f_requestId;
		//bool f_httpRequestAborted;
	signals:
		void requestDone(/*int exit_code*/);
	protected slots:
		//void replyFinished(QNetworkReply *reply);
		//void replyError(QNetworkReply::NetworkError);
		//void replyReadyRead();
		//void responseHeaderReceived ( const QHttpResponseHeader & resp );
		//void requestFinished(int id, bool error);
		void timeOut();
	public:
		QVariantMap postWaitForAnswer(const QUrl &url, const QByteArray &data, int timeout_ms = -1);
		QString lastError() {return f_lastError;}
		QNetworkAccessManager* http() {return f_http;}
	public:
		QFHttpMySqlDriverHttp(QObject *parent);
		//~QFHttpMySqlDriverHttp() {}

};

class QFHttpMySqlDriverResult : public QSqlResult
{
	protected:
		QSqlRecord fields;
		typedef QVariantList Row;
		QList<Row> rows;
		int f_numRowsAffected;
		QVariant f_lastInsertId;
		//QString f_lastQuery;
	protected:
		virtual QVariant lastInsertId () const {return f_lastInsertId;}
		//QString lastQuery () const {return f_lastQuery;}
	protected:
        const QFHttpMySqlDriver * driver() const;
		//QFHttpMySqlDriver * driver_ref() const {return const_cast<QFHttpMySqlDriver*>(driver());};
	protected:
		virtual QVariant data(int index);
		virtual bool isNull(int index);
		virtual bool reset(const QString &qs);
		virtual bool fetch(int index);
		virtual bool fetchFirst();
		virtual bool fetchLast();
		virtual int size();
		virtual int numRowsAffected();
		virtual QSqlRecord record() const;
	public:
		QFHttpMySqlDriverResult(const QSqlDriver *driver);
		~QFHttpMySqlDriverResult();

};

class QFHttpMySqlDriver : public QSqlDriver
{
	Q_OBJECT
	friend class QFHttpMySqlDriverResult;
	protected:
		struct OpenParams {
			QString db;
			QString user;//, httpUser;
			QString password;//, httpPassword;
			QString host;//, httpHost;
			int port;//, httpPort;
			//QString httpPath;
			QString options;

			OpenParams() : port(0) {}//, httpPort(80) {}
		};
		OpenParams openParams;
		QFHttpMySqlDriverHttp *f_http;
	protected:
		QFHttpMySqlDriverHttp *http() const;
		//static const QString EXEC_SQL_PATH_PREFIX;
	public:
		void setCurrentDatabase(const QString &db) {openParams.db = db;}
		virtual bool hasFeature(DriverFeature feature) const;
		virtual bool open(const QString &db, const QString &user, const QString &password, const QString &host, int port, const QString &options);
		virtual void close();
		virtual QSqlResult *createResult() const;
		virtual QSqlIndex primaryIndex ( const QString & table_name ) const;
		virtual QSqlRecord record ( const QString & table_name ) const;
		QUrl url() const;
	public:
		QFHttpMySqlDriver(QObject *parent = NULL);
		virtual ~QFHttpMySqlDriver();
};

#endif // QFHTTPMYSQL_H

