#include "racomclient.h"
#include "racomclientwidget.h"

#include "../Event/eventplugin.h"

#include <quickevent/core/si/checkedcard.h>

#include <qf/qmlwidgets/framework/mainwindow.h>
#include <qf/qmlwidgets/dialogs/dialog.h>

#include <qf/core/log.h>
#include <qf/core/assert.h>
#include <qf/core/sql/querybuilder.h>
#include <qf/core/sql/query.h>
#include <qf/core/sql/connection.h>

#include <QFile>
#include <QSettings>
#include <QStandardPaths>
#include <QUdpSocket>

namespace qfc = qf::core;
namespace qfw = qf::qmlwidgets;
namespace qfd = qf::qmlwidgets::dialogs;
//namespace qfm = qf::core::model;
namespace qfs = qf::core::sql;

namespace services {

//static auto SETTING_KEY_FILE_NAME = QStringLiteral("fileName");

RacomClient::RacomClient(QObject *parent)
	: Super(RacomClient::serviceName(), parent)
{
	connect(eventPlugin(), &Event::EventPlugin::dbEventNotify, this, &RacomClient::onDbEventNotify, Qt::QueuedConnection);
}

QString RacomClient::serviceName()
{
	return QStringLiteral("RacomClient");
}

void RacomClient::onDbEventNotify(const QString &domain, int connection_id, const QVariant &data)
{
	Q_UNUSED(connection_id)
	Q_UNUSED(data)
	if(domain == QLatin1String(Event::EventPlugin::DBEVENT_PUNCH_RECEIVED)) {
		//onRacomDataReceived(data.toMap());
	}
}

void RacomClient::onUdpSocketReadyRead()
{
	if(status() != Status::Running)
		return;
	QUdpSocket *sock = udpSocket();
	QByteArray data = sock->readAll();
	qfInfo() << data;
}

qf::qmlwidgets::framework::DialogWidget *RacomClient::createDetailWidget()
{
	auto *w = new RacomClientWidget();
	return w;
}

QUdpSocket *RacomClient::udpSocket()
{
	if(!m_udpSocket) {
		m_udpSocket = new QUdpSocket(this);
		RacomClientSettings ss = settings();
		if(m_udpSocket->bind(ss.port())) {
			connect(m_udpSocket, &QUdpSocket::readyRead, this, &RacomClient::onUdpSocketReadyRead);
			connect(m_udpSocket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error), [](QAbstractSocket::SocketError) {
				qfError() << "RacomClient: socket error";
			});
		}
		else {
			qfError() << "Cannot bind UDP socket to port:" << ss.port();
		}
	}
	return m_udpSocket;
}

} // namespace services
