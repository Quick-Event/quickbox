
//
// Author: Frantisek Vacek <fanda.vacek@volny.cz>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//

#include "qfhttpmysql.h"
//#include "theapp.h"

#include <QSqlQuery>
#include <QSqlField>
#include <QSqlIndex>
#include <QSqlError>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QTimer>
#include <QBuffer>
#include <QUrl>
#include <QJsonDocument>
#include <QEventLoop>

#include <qf/core/log.h>


//=================================================
//         QFHttpMySqlDriverHttp
//=================================================
QFHttpMySqlDriverHttp::QFHttpMySqlDriverHttp(QObject * parent)
	: QObject(parent)
{
	//f_responseBuffer = new QBuffer(this);
	//f_eventLoop = NULL;
	//f_currentReply = NULL;

	f_http = new QNetworkAccessManager(this);
	//connect(f_http, SIGNAL(requestFinished(int, bool)), this, SLOT(requestFinished(int, bool)));
	//connect(f_http, SIGNAL(responseHeaderReceived(const QHttpResponseHeader &)), this, SLOT(responseHeaderReceived(QHttpResponseHeader)));

	f_timeoutTimer = new QTimer(this);
	connect(f_timeoutTimer, SIGNAL(timeout()), this, SLOT(timeOut()));
}
/*
void QFHttpMySqlDriverHttp::replyFinished(QNetworkReply* reply)
{
	f_lastError = QString();
	if(reply->error() == QNetworkReply::NoError) {
		f_replyBuffer = reply->readAll();
	}
	else {
		f_lastError = reply->errorString();
	}
	reply->deleteLater();
	emit requestDone();
}
*/
QVariantMap QFHttpMySqlDriverHttp::postWaitForAnswer(const QUrl &url, const QByteArray & data, int timeout_ms)
{
	qfLogFuncFrame();
	/*
	QUrl url;
	url.setScheme("http");
	url.setHost("localhost");
	url.setPort(8888);
	url.setPath(path);
	*/
	QNetworkRequest request;
	request.setUrl(url);
	qfDebug() << "\t posting to:" << url.toString() << " data:" << data;
	QNetworkReply *reply = http()->post(request, data);
	connect(f_http, SIGNAL(finished(QNetworkReply*)), this, SIGNAL(requestDone()));
	//connect(reply, SIGNAL(readyRead()), this, SLOT(replyReadyRead()));
	//connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(replyError(QNetworkReply::NetworkError)));
	//connect(reply, SIGNAL(sslErrors(QList<QSslError>)), this, SLOT(slotSslErrors(QList<QSslError>)));

	f_lastError = QString();
	//SAFE_DELETE(f_eventLoop);
	//f_eventLoop = new QEventLoop(this);
	QEventLoop ev_loop;
	connect(this, SIGNAL(requestDone()), &ev_loop, SLOT(quit()));
	if(timeout_ms == -1) timeout_ms = 5000; /// default timeout je 5s
	if(timeout_ms > 0) f_timeoutTimer->start(timeout_ms);
	else f_timeoutTimer->stop();
	qfDebug() << "\t starting event loop to wait for answer";
	ev_loop.exec(QEventLoop::ExcludeUserInputEvents);
	QVariantMap ret;
	if(lastError().isEmpty()) {
		if(!reply->isFinished()) {
			/// to by mel zvladnout uz timeout
			f_lastError = tr("Unfinished reply.");
			qfError() << lastError();
		}
		else if(reply->error() == QNetworkReply::NoError) {
			QByteArray ba = reply->readAll();
			QString s = QString::fromUtf8(ba);
			QJsonDocument jsd = QJsonDocument::fromJson(ba);
			ret = jsd.toVariant().toMap();
			/*
			if(ret.isEmpty()) {
				f_lastError = tr("Invalid server response.");
				QF_EXCEPTION(lastError());
			}
			else*/
			if(ret.contains("exception")) {
				f_lastError = ret.value("exception").toMap().value("errmsg", tr("Unknown error")).toString();
				qfError() << lastError();
			}
		}
		else {
			f_lastError = reply->errorString();
			qfError() << lastError();
		}
	}
	else {
		qfError() << lastError();
	}
	reply->deleteLater();
	//qfDebug() << "return:" << QFJson::variantToString(ret);
	return ret;
}

void QFHttpMySqlDriverHttp::timeOut()
{
	f_lastError = tr("HTTP response timeout");
	emit requestDone();
	//if(f_eventLoop) f_eventLoop->exit(-1);
}
/*
void QFHttpMySqlDriverHttp::responseHeaderReceived(const QHttpResponseHeader & resp_header)
{
	switch (resp_header.statusCode()) {
		case 200:                   /// Ok
		case 301:                   /// Moved Permanently
		case 302:                   /// Found
		case 303:                   /// See Other
		case 307:                   /// Temporary Redirect
         	/// these are not error conditions
		break;

		default:
			f_lastError = resp_header.reasonPhrase();
			//f_httpRequestAborted = true;
			f_http->abort();
	}
}

void QFHttpMySqlDriverHttp::requestFinished(int request_id, bool error)
{
	if (f_requestId != request_id) return;
	int exit_code = 0;
	if (error) {
		if(f_lastError.isEmpty()) f_lastError = f_http->errorString();
		exit_code = -1;
	}
	else {
	}
	emit requestDone(exit_code);
	//if(f_eventLoop) f_eventLoop->exit(exit_code);
}
*/
//=================================================
//                                    QFHttpMySqlDriverResult
//=================================================
QFHttpMySqlDriverResult::QFHttpMySqlDriverResult(const QSqlDriver * driver)
	: QSqlResult(driver)
{
	setAt(QSql::BeforeFirstRow);
	f_numRowsAffected = 0;
}

QFHttpMySqlDriverResult::~ QFHttpMySqlDriverResult()
{
}

const QFHttpMySqlDriver * QFHttpMySqlDriverResult::driver() const
{
	return qobject_cast<const QFHttpMySqlDriver*>(QSqlResult::driver());
}

QVariant QFHttpMySqlDriverResult::data(int ix)
{
	//qfLogFuncFrame() << "ix:" << ix;
	return fields.value(ix);
}

bool QFHttpMySqlDriverResult::isNull(int ix)
{
	//qfLogFuncFrame() << "ix:" << ix;
	return fields.isNull(ix);
}

bool QFHttpMySqlDriverResult::reset(const QString & qs)
{
	qfLogFuncFrame();
	bool ret = true;
	f_numRowsAffected = 0;
	f_lastInsertId = 0;
	fields = QSqlRecord();
	rows.clear();
	setActive(false);
	setSelect(false);
	setAt(QSql::BeforeFirstRow);
	//f_lastQuery = qs;
	//qfInfo() << qs;
	QFHttpMySqlDriverHttp *http = driver()->http();
	{
		//if (qs.startsWith("USE ")) { driver_ref()->openParams.db = qs.section(' ', -1); }
		QByteArray ba;
		{
			QVariantMap m;
			m["query"] = qs;

            QVariantMap m2;
            m2["database"] = driver()->openParams.db;
            m2["user"] = driver()->openParams.user;
            m2["password"] = driver()->openParams.password;
            //m2["host"] = host;
            //m2["port"] = port;
            m2["options"] = driver()->openParams.options;
            m["open"] = m2;
			QJsonDocument jsd = QJsonDocument::fromVariant(m);
			ba = jsd.toJson();
		}
		QVariantMap resp = http->postWaitForAnswer(driver()->url(), ba);
		setActive(true);
		//qfDebug() << "resp:" << QFJson::variantToString(resp);
		QVariantMap resultset = resp.value("resultset").toMap();
		setSelect(!resultset.isEmpty());
		if(isSelect()) {
			foreach(QVariant v, resultset.value("fields").toList()) {
				QVariantMap m = v.toMap();
				qfDebug() << "field name:" << m.value("name").toString() << "type:" << m.value("type").toString();
				QSqlField fld(m.value("name").toString(), QVariant::nameToType(m.value("type").toString().toLatin1().constData()));
				fields.append(fld);
			}
			foreach(QVariant v, resultset.value("rows").toList()) {
				rows << v.toList();
			}
		}
		else {
			f_lastInsertId = resultset.value("lastInsertId");
			f_numRowsAffected = resultset.value("numRowsAffected").toInt();
		}
	}
	/*
	catch(QFException &e) {
		ret = false;
		QSqlError err("QFHttpMySqlDriverResult", e.msg(), QSqlError::StatementError);
		setLastError(err);
	}
	*/
	return ret;
}

bool QFHttpMySqlDriverResult::fetch(int ix)
{
	//qfLogFuncFrame() << "ix:" << ix;
	bool ret = false;
	if(ix < 0) {
		setAt(QSql::BeforeFirstRow);
	}
	else if(ix < rows.count()) {
		setAt(ix);
		for(int i=0; i<fields.count(); i++) {
			fields.setValue(i, rows.value(ix).value(i));
		}
		ret = true;
	}
	else {
		setAt(QSql::AfterLastRow);
	}
	return ret;
}

bool QFHttpMySqlDriverResult::fetchFirst()
{
	//qfLogFuncFrame();
	return fetch(0);
}

bool QFHttpMySqlDriverResult::fetchLast()
{
	//qfLogFuncFrame();
	return fetch(rows.count() - 1);
}

int QFHttpMySqlDriverResult::size()
{
	//qfLogFuncFrame();
	return rows.count();
}

int QFHttpMySqlDriverResult::numRowsAffected()
{
	//qfInfo() << "numRowsAffected:" << f_numRowsAffected;
	return f_numRowsAffected;
}

QSqlRecord QFHttpMySqlDriverResult::record() const
{
	//qfLogFuncFrame();
	return fields;
}

//=================================================
//                                    QFHttpMySqlDriver
//=================================================
//const QString QFHttpMySqlDriver::EXEC_SQL_PATH_PREFIX = INJ_EXEC_SQL_PATH_PREFIX;

QFHttpMySqlDriver::QFHttpMySqlDriver(QObject *parent)
	: QSqlDriver(parent)
{
	f_http = new QFHttpMySqlDriverHttp(this);
}

QFHttpMySqlDriver::~QFHttpMySqlDriver()
{
}

bool QFHttpMySqlDriver::hasFeature(QSqlDriver::DriverFeature feature) const
{
	static QSet<QSqlDriver::DriverFeature> features;
	if(features.isEmpty()) features << QSqlDriver::QuerySize << QSqlDriver::Unicode << QSqlDriver::LastInsertId;
	return features.contains(feature);
}

QUrl QFHttpMySqlDriver::url() const
{
	QUrl url;
	url.setScheme("http");
	url.setHost(openParams.host);
	url.setPort(openParams.port);
	//url.setPath(openParams.db);
    url.setPath("/qsqlmon_proxy.php");
	return url;
}

bool QFHttpMySqlDriver::open(const QString & db, const QString & user, const QString & password, const QString & host, int port, const QString & options)
{
	openParams.db = db;
	openParams.user = user;
	openParams.password = password;
	openParams.host = host;
	openParams.port = port;
	openParams.options = options;
	/*
	QUrl url(host);
	openParams.httpHost = url.host();
	openParams.httpPort = url.port();
	if(openParams.httpPort == 0) openParams.httpPort = 80;
	openParams.httpPath = url.path();
	openParams.httpUser = url.userName();
	openParams.httpPassword = url.password();
	http()->setHost(openParams.httpHost);
	http()->setPort(openParams.httpPort);
	*/
	QByteArray json_data;
	{
		QVariantMap m2;
		m2["database"] = db;
		m2["user"] = user;
		m2["password"] = password;
		m2["host"] = host;
		m2["port"] = port;
		m2["options"] = options;
		QVariantMap m;
		m["open"] = m2;
		QJsonDocument jsd = QJsonDocument::fromVariant(m);
		json_data = jsd.toJson();
	}
	//try
	{
		http()->postWaitForAnswer(url(), json_data);
        qDebug() << "open succeeded..." << endl;
		setOpen(true);
		setOpenError(false);
	}
	/*
	catch(QFException &e) {
		setOpenError(true);
		QSqlError err("QFHttpMySqlDriver", e.msg(), QSqlError::ConnectionError);
		setLastError(err);
	}
	*/
	return true;
}

void QFHttpMySqlDriver::close()
{
	QByteArray json_data;
	{
		QVariantMap m;
		m["close"] = 0;
		QJsonDocument jsd = QJsonDocument::fromVariant(m);
		json_data = jsd.toJson();
	}
	/*
	try {
		http()->postWaitForAnswer(url(), json_data);
	}
	catch(QFException &e) {
		QSqlError err("QFHttpMySqlDriver", e.msg(), QSqlError::ConnectionError);
		setLastError(err);
	}
	*/
	http()->postWaitForAnswer(url(), json_data);

	setOpen(false);
}

QSqlResult * QFHttpMySqlDriver::createResult() const
{
	return new QFHttpMySqlDriverResult(this);
}

QSqlIndex QFHttpMySqlDriver::primaryIndex(const QString & table_name) const
{
	QSqlIndex idx;
	if (!isOpen()) return idx;

	QSqlQuery q(createResult());
	QString stmt(QLatin1String("show index from %1;"));
	QSqlRecord rec = record(table_name);
	q.exec(stmt.arg(escapeIdentifier(table_name, QSqlDriver::TableName)));
	while (q.next()) {
		if (q.value(2).toString() == QLatin1String("PRIMARY")) {
			idx.append(rec.field(q.value(4).toString()));
			idx.setCursorName(q.value(0).toString());
			idx.setName(q.value(2).toString());
		}
	}
	return idx;
}

QSqlRecord QFHttpMySqlDriver::record(const QString & table_name) const
{
	QSqlRecord info;
	if (!isOpen()) return info;
	/// mysql_list_fields() nemuze mit parametr ve tvaru 'schemaname.tablename', jenom 'tablename' :((
	QString s = QLatin1String("SELECT * FROM %1 WHERE FALSE");
	QSqlQuery q(createResult());
	q.exec(s.arg(table_name)); q.next();
//fprintf(stderr, "\t'%s'\n", s.arg(tablename).toStdString().c_str());
	QSqlRecord r = q.record();
	for(int i=0; i<r.count(); i++) {
		QSqlField f = r.field(i);
		s = f.name();
		int ix = s.lastIndexOf(QLatin1String("."));
		if(ix >= 0) {
			s = s.mid(ix + 1);
			f.setName(s);
		}
		info.append(f);
		//fprintf(stderr, "\t\t'%s'\n", f.name().toStdString().c_str());
	}
	return info;
}

QFHttpMySqlDriverHttp * QFHttpMySqlDriver::http() const
{
	return f_http;
}
