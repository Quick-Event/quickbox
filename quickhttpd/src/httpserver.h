#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <QTcpServer>

class HttpServer : public QTcpServer
{
	Q_OBJECT
public:
	HttpServer(QObject *parent);
private:
	void onNewConnection();
};

#endif // HTTPSERVER_H
