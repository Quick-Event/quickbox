#include "racomclient.h"
#include "racomclientwidget.h"

#include "../CardReader/cardreaderplugin.h"

#include <Event/eventplugin.h>

#include <quickevent/core/si/checkedcard.h>

#include <siut/sidevicedriver.h>
#include <siut/sidevicedriver.h>

#include <qf/qmlwidgets/framework/mainwindow.h>
#include <qf/qmlwidgets/dialogs/dialog.h>

#include <qf/core/log.h>
#include <qf/core/assert.h>
#include <qf/core/sql/querybuilder.h>
#include <qf/core/sql/query.h>
#include <qf/core/sql/connection.h>

#include <QFile>
#include <QNetworkDatagram>
#include <QSettings>
#include <QStandardPaths>
#include <QUdpSocket>

namespace qfc = qf::core;
namespace qfw = qf::qmlwidgets;
namespace qfd = qf::qmlwidgets::dialogs;
namespace qff = qf::qmlwidgets::framework;
//namespace qfm = qf::core::model;
namespace qfs = qf::core::sql;

namespace services {

static CardReader::CardReaderPlugin *cardReaderPlugin()
{
	qff::MainWindow *fwk = qff::MainWindow::frameWork();
	auto ret = qobject_cast<CardReader::CardReaderPlugin *>(fwk->plugin("CardReader"));
	//QF_ASSERT(ret != nullptr, "Bad plugin", return 0);
	return ret;
}

RacomClient::RacomClient(QObject *parent)
	: Super(RacomClient::serviceName(), parent)
{
	connect(eventPlugin(), &Event::EventPlugin::dbEventNotify, this, &RacomClient::onDbEventNotify, Qt::QueuedConnection);
}

void RacomClient::run()
{
	udpSocket();
	Super::run();
}

void RacomClient::stop()
{
	Super::stop();
	QF_SAFE_DELETE(m_udpSocket);
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
	QNetworkDatagram dg = sock->receiveDatagram();
	QByteArray data = dg.data();
	qfInfo() << data.toHex();
	// remove possible initial garbage
	int i = 0;
	for(i=0; i<data.size(); i++)
		if(data.at(i) == 0x02)
			break;
	data = data.mid(i);
	// remove possible multiple 0x02
	for(i=1; i<data.size(); i++)
		if(data.at(i) != 0x02)
			break;
	data = data.mid(i-1);
	qfInfo() << "stripped data:" << data.toHex();
	if(!m_siDriver) {
		m_siDriver = new siut::DeviceDriver(this);
		CardReader::CardReaderPlugin *plugin = cardReaderPlugin();
		if(plugin) {
			//connect(m_siDriver, &siut::DeviceDriver::siMessageReceived, plugin, &CardReader::CardReaderPlugin::emitSiMessageReceived);
		}
	}
	m_siDriver->processData(data);
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
		qfDebug() << ss;
		if(m_udpSocket->bind(ss.port())) {
			connect(m_udpSocket, &QUdpSocket::readyRead, this, &RacomClient::onUdpSocketReadyRead);
			connect(m_udpSocket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error), [](QAbstractSocket::SocketError) {
				qfError() << "RacomClient: socket error";
			});
		}
		else {
			qfError() << "Cannot bind UDP socket to port:" << ss.port();
		}
		qfInfo() << "UDP socket listenning on port:" << ss.port();
	}
	return m_udpSocket;
}

} // namespace services
