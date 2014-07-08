#include "networkreply.h"

#include <qf/core/log.h>

#include <QNetworkReply>

using namespace qf::core::qml;

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
		/*
		qfDebug() << "raw header pairs:";
		for(auto p : m_reply->rawHeaderPairs()) {
			qfDebug() << p.first << p.second;
		}
		qfDebug() << "raw header pairs list end";
		*/
		/// !!! THIS IS A MEMORY LEAK !!!
		/// don't know why, but deleting m_reply causes segmentation fault in the QNetworkHeadersPrivate destructor
		/// watch https://bugreports.qt-project.org/browse/QTBUG-40125
		//m_reply->deleteLater();
	}
}

void qf::core::qml::NetworkReply::setReply(QNetworkReply *repl)
{
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

QString qf::core::qml::NetworkReply::errorString() const
{
	QString ret;
	if(m_reply) {
		ret = m_reply->errorString();
	}
	return ret;
}

QString qf::core::qml::NetworkReply::textData() const
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

void qf::core::qml::NetworkReply::downloadProgress_helper(qint64 bytes_received, qint64 bytes_total)
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

void qf::core::qml::NetworkReply::finished_helper()
{
	if(m_reply) {
		emit finished(m_reply->error() == QNetworkReply::NoError);
		emit downloadProgress(m_reply->url().toString(), 100, 100);
	}
}

void qf::core::qml::NetworkReply::readyRead_helper()
{
	if(m_reply) {
		m_data += m_reply->readAll();
	}
}
