#ifndef QF_CORE_NETWORK_NETWORKREPLY_H
#define QF_CORE_NETWORK_NETWORKREPLY_H

#include "../core/coreglobal.h"

#include <QObject>
#include <QByteArray>
#include <QUrl>

class QNetworkReply;

namespace qf {
namespace core {
namespace network {

class QFCORE_DECL_EXPORT NetworkReply : public QObject
{
	Q_OBJECT
	Q_PROPERTY(QUrl url READ url)
	Q_PROPERTY(QString errorString READ errorString)
	Q_PROPERTY(QString textData READ textData)
public:
	explicit NetworkReply(QObject *parent = nullptr);
	~NetworkReply() Q_DECL_OVERRIDE;
public:
	Q_SIGNAL void downloadProgress(const QString &url, int processed, int total);
	Q_SIGNAL void finished(bool ok);

	void setReply(QNetworkReply *repl);

	QString errorString() const;
	QByteArray data() const {return m_data;}
	QString textData() const;
	QUrl url() const;
private:
	Q_SLOT void downloadProgress_helper(qint64 bytes_received, qint64 bytes_total);
	Q_SLOT void finished_helper();
	Q_SLOT void readyRead_helper();
private:
	QNetworkReply *m_reply;
	QByteArray m_data;
};

}}}

#endif // QF_CORE_NETWORK_NETWORKREPLY_H
