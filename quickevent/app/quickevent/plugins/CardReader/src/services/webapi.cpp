#include "webapi.h"
#include "webapiwidget.h"

#include "../cardreaderplugin.h"

#include <quickevent/core/si/checkedcard.h>
#include <quickevent/core/codedef.h>

#include <qf/qmlwidgets/framework/mainwindow.h>

#include <qf/core/log.h>

#include <QTcpSocket>
#include <regex>

using qf::qmlwidgets::framework::getPlugin;

namespace CardReader {
namespace services {

WebApi::WebApi(QObject *parent)
	: Super(WebApi::serviceName(), parent)
{
	connect(this, &WebApi::settingsChanged, this, &WebApi::init, Qt::QueuedConnection);
}

void WebApi::run()
{
	init();
	Super::run();
}

void WebApi::stop()
{
	Super::stop();
	m_tcpClients.clear();
	m_tcpServer.reset();
}

QString WebApi::serviceName()
{
	return QStringLiteral("WebApi");
}

qf::qmlwidgets::framework::DialogWidget *WebApi::createDetailWidget()
{
	auto *w = new WebApiWidget();
	return w;
}

void WebApi::init()
{
	WebApiSettings ss = settings();
	int port = ss.tcpListenPort();

	m_tcpClients.clear();
	m_tcpServer.reset(new QTcpServer(this));
	connect(m_tcpServer.get(), &QTcpServer::newConnection, this, &WebApi::onNewConnection);

	if (!m_tcpServer->listen(QHostAddress::Any, port)) {
		qfError() << "Server could not start listening port " << port << ": " << m_tcpServer->errorString().toLocal8Bit();
	} else {
		qfInfo() << "TCP server started listening port " << port;
	}
}

void WebApi::onNewConnection()
{
	while (m_tcpServer->hasPendingConnections())
	{
		QTcpSocket *socket = m_tcpServer->nextPendingConnection();

		connect(socket, &QTcpSocket::readyRead, this, &WebApi::onReadyRead);
		connect(socket, &QTcpSocket::disconnected, this, [this] {
			QTcpSocket *socket = static_cast<QTcpSocket*>(sender());
			m_tcpClients.erase(socket);
			socket->deleteLater();
		});
		m_tcpClients.emplace(socket, new QByteArray());
	}
}

void WebApi::onReadyRead()
{
	QTcpSocket *socket = static_cast<QTcpSocket*>(sender());
	auto sendError = [socket](const char *error) {
		socket->write("HTTP/1.1 ");
		socket->write(error);
		socket->write("\r\n\r\n");
		socket->flush();
		socket->waitForBytesWritten(3000);
		socket->close();
	};
	auto it = m_tcpClients.find(socket);
	QByteArray *buffer = it->second.get();
	while (socket->bytesAvailable() > 0)
	{
		buffer->append(socket->readAll());

		// Parse the first request line
		int eol = buffer->indexOf("\r\n");
		if (-1 == eol) {
			continue;
		}
		const static std::regex rex("^([A-Z]+) ([^:]+://[^/]*|)([^ ]+)");
		std::cmatch match;
		if (!std::regex_search(buffer->constData(), buffer->constData() + eol, match, rex)) {
			sendError("405 Method Not Allowed");
			continue;
		}
		if (match[1].str() != "POST") {
			sendError("405 Method Not Allowed");
			continue;
		}
		if (match[3].str() != "/card") {
			sendError("404 Not Found");
			continue;
		}

		// Figure out the whole request length and wait for it
		const std::string SIZE = "Content-Length: ";
		int lbegin = buffer->indexOf(SIZE.data());
		if (-1 == lbegin) {
			if (-1 != buffer->indexOf("\r\n\r\n")) {
				sendError("405 Method Not Allowed");
			}
			continue;
		}
		lbegin += SIZE.size();
		int lend = buffer->indexOf("\r", lbegin);
		if (-1 == lend) {
			continue;
		}
		std::string clength(buffer->constData() + lbegin, buffer->constData() + lend);
		int len = std::stoi(clength);
		int dataIdx = buffer->indexOf("\r\n\r\n");
		if (-1 == dataIdx) {
			continue;
		}
		dataIdx += 4;
		if (buffer->size() < dataIdx + len) {
			continue;
		}

		// Process POST /card
		QByteArray data{buffer->mid(dataIdx, len)};
		QJsonParseError parseError{};
		QJsonDocument jsonDoc = QJsonDocument::fromJson(data, &parseError);
		if (jsonDoc.isNull()) {
			socket->write("HTTP/1.1 400 Bad Request\r\n\r\n");
			socket->write("JSON error at the offset ");
			socket->write(std::to_string(parseError.offset).c_str());
			socket->write(":\n");
			socket->write(parseError.errorString().toLocal8Bit());
			socket->flush();
			socket->waitForBytesWritten(3000);
			socket->close();
			break;
		}
		onTcpReadoutReceived(jsonDoc.toVariant());
		socket->write("HTTP/1.1 200 OK\r\n\r\n");
		socket->flush();
		socket->waitForBytesWritten(3000);
		socket->close();
		break;
	}
}

void WebApi::onTcpReadoutReceived(const QVariant &data)
{
	if (!m_siDriver) {
		m_siDriver.reset(new siut::DeviceDriver(this));
		connect(m_siDriver.get(), &siut::DeviceDriver::siTaskFinished, getPlugin<CardReaderPlugin>(), &CardReader::CardReaderPlugin::emitSiTaskFinished);
	}
	emit m_siDriver->siTaskFinished(static_cast<int>(siut::SiTask::Type::CardRead), data);
	siut::SICard card(data.toMap());
	for (int i = 0, n = card.punchCount(); i < n; ++i) {
		emit m_siDriver->siTaskFinished(static_cast<int>(siut::SiTask::Type::Punch), card.punchAt(i));
	}
}

}}
