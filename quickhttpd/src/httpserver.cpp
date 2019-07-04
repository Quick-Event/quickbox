#include "httpserver.h"
#include "httpconnection.h"

#include <qf/core/log.h>

HttpServer::HttpServer(QObject *parent)
	: QTcpServer(parent)
{
	connect(this, &HttpServer::newConnection, this, &HttpServer::onNewConnection);
}

void HttpServer::onNewConnection()
{
	while(QTcpSocket *sock = nextPendingConnection()) {
		qfInfo() << "accepting connection, socket:" << sock;
		new HttpConnection(sock, this);
	}
}
