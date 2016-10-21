#include "networkreply.h"
#include "../core/log.h"

#include <QNetworkReply>

namespace qf {
namespace core {
namespace network {

NetworkReply::NetworkReply(QObject *parent) :
	QObject(parent), m_reply(nullptr)
{
}

NetworkReply::~NetworkReply()
{
	qfLogFuncFrame() << this;
	if(m_reply) {
		m_reply->abort();
		m_reply->close();
#if defined __clang__ && __clang_major__ <= 3 &&  __clang_minor__ <= 4
		#warning "mem leak caused by QTBUG-40125 is still here"
		/// !!! THIS IS A MEMORY LEAK !!!
		/// don't know why, but deleting m_reply causes segmentation fault in the QNetworkHeadersPrivate destructor
		/// watch https://bugreports.qt-project.org/browse/QTBUG-40125
		/// it's a clang 3.4 bug.
		qfWarning() << "mem leak caused by QTBUG-40125 is still here";
#else
		m_reply->deleteLater();
#endif
	}
	emit downloadProgress(QString(), 0, 0);
}

void NetworkReply::setReply(QNetworkReply *repl)
{
	qfLogFuncFrame() << repl << (repl? repl->url().toString(): QString()) << "finished:" << (repl? repl->isFinished(): true);
	m_reply = repl;
	m_reply->setParent(nullptr);
	m_data.clear();
	connect(m_reply, &QNetworkReply::downloadProgress, this, &NetworkReply::downloadProgress_helper);
	connect(m_reply, &QNetworkReply::finished, this, &NetworkReply::finished_helper);
	connect(m_reply, &QNetworkReply::readyRead, this, &NetworkReply::readyRead_helper);
	// network reply cannot be connected now, so postpone signal emit to next event loop
	QMetaObject::invokeMethod(this, "downloadProgress_helper", Qt::QueuedConnection,
							  Q_ARG(qint64, 0),
							  Q_ARG(qint64, 100));
}

QString NetworkReply::errorString() const
{
	QString ret;
	if(m_reply) {
		ret = m_reply->errorString();
	}
	return ret;
}

QString NetworkReply::textData() const
{
	return QString::fromUtf8(m_data);
}

QUrl NetworkReply::url() const
{
	QUrl ret;
	if(m_reply) {
		ret = m_reply->url();
	}
	return ret;
}

void NetworkReply::downloadProgress_helper(qint64 bytes_received, qint64 bytes_total)
{
	qfLogFuncFrame() << bytes_received << "of" << bytes_total;
	if(m_reply) {
		int completed = bytes_received;
		int total = bytes_total;
		if(bytes_total < 0) {
			/// dest size is not known
			total = 3 * completed;
		}
		emit downloadProgress(QString("%1/%2 %3").arg(completed).arg(total).arg(m_reply->url().toString()), completed, total);
	}
}

void NetworkReply::finished_helper()
{
	if(m_reply) {
		emit finished(m_reply->error() == QNetworkReply::NoError);
		emit downloadProgress(m_reply->url().toString(), 100, 100);
	}
}

void NetworkReply::readyRead_helper()
{
	qfLogFuncFrame();
	if(m_reply) {
		int sz = m_data.length();
		m_data += m_reply->readAll();
		qfDebug() << "Chunk of" << (m_data.length() - sz) << "read.";
	}
}

}}}
