#ifndef HTTPCONNECTION_H
#define HTTPCONNECTION_H

#include <QTcpSocket>

class HttpConnection : public QObject
{
	Q_OBJECT
public:
	HttpConnection(QTcpSocket *sock, QObject *parent);
	~HttpConnection() override;

	void onReadyRead();
private:
	void processRequest();
	void responseOK(const QByteArray &message);
private:
	QTcpSocket *m_socket;
	QString m_getPath;
};

#endif // HTTPCONNECTION_H
