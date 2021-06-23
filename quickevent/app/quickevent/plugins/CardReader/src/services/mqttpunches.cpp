#include "mqttpunches.h"
#include "mqttpuncheswidget.h"

#include "../cardreaderplugin.h"
#include <quickevent/core/si/checkedcard.h>

//#include <siut/simessagedata.h>
#include <siut/sidevicedriver.h>

#include <qf/qmlwidgets/framework/mainwindow.h>
#include <qf/qmlwidgets/dialogs/dialog.h>

#include <qf/core/log.h>
#include <qf/core/assert.h>
#include <qf/core/sql/querybuilder.h>
#include <qf/core/sql/query.h>
#include <qf/core/sql/connection.h>

#include <plugins/Event/src/eventplugin.h>

#include <QFile>
#include <QSettings>
#include <QStandardPaths>
#include <QTcpServer>
#include <QUdpSocket>
#include <QTcpSocket>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 8, 0))
#include <QNetworkDatagram>
#endif

namespace qfc = qf::core;
namespace qfw = qf::qmlwidgets;
namespace qfd = qf::qmlwidgets::dialogs;
namespace qff = qf::qmlwidgets::framework;
//namespace qfm = qf::core::model;
namespace qfs = qf::core::sql;
using qf::qmlwidgets::framework::getPlugin;
using CardReader::CardReaderPlugin;

namespace CardReader {
namespace services {

void MqttPunches::run()
{
	init();
	Super::run();
}

void MqttPunches::stop()
{
	Super::stop();
	QF_SAFE_DELETE(m_rawSIDataUdpSocket);
}

QString MqttPunches::serviceName()
{
	return QStringLiteral("MqttPunches");
}

void MqttPunches::onDbEventNotify(const QString &domain, int connection_id, const QVariant &data)
{
	Q_UNUSED(connection_id)
	Q_UNUSED(data)
	if(domain == QLatin1String(Event::EventPlugin::DBEVENT_PUNCH_RECEIVED)) {
		//onRacomDataReceived(data.toMap());
	}
}

void MqttPunches::onRawSIDataUdpSocketReadyRead()
{
	if(status() != Status::Running)
		return;
#if (QT_VERSION >= QT_VERSION_CHECK(5, 8, 0))
	QUdpSocket *sock = rawDataUdpSocket();
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
		connect(m_siDriver, &siut::DeviceDriver::siTaskFinished, getPlugin<CardReaderPlugin>(), &CardReader::CardReaderPlugin::emitSiTaskFinished);
	}
	m_siDriver->processData(data);
#else
	qfWarning() << "Racom client is not supported with Qt < 5.8";
#endif
}

qf::qmlwidgets::framework::DialogWidget *MqttPunches::createDetailWidget()
{
	auto *w = new MqttPunchesWidget();
	return w;
}

QUdpSocket *MqttPunches::rawDataUdpSocket()
{
	return m_rawSIDataUdpSocket;
}

void MqttPunches::init()
{
	/*
	QF_SAFE_DELETE(m_rawSIDataUdpSocket);
	QF_SAFE_DELETE(m_sirxdDataServer);
	MqttPunchesSettings ss = settings();
	qfDebug() << ss;

	if(ss.isListenRawData()) {
		m_rawSIDataUdpSocket = new QUdpSocket(this);
		if(m_rawSIDataUdpSocket->bind(ss.rawDataListenPort())) {
			connect(m_rawSIDataUdpSocket, &QUdpSocket::readyRead, this, &MqttPunches::onRawSIDataUdpSocketReadyRead);
			connect(m_rawSIDataUdpSocket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error), [](QAbstractSocket::SocketError) {
				qfError() << "MqttPunches: raw SI data UDP socket error";
			});
		}
		else {
			qfError() << "Cannot bind raw SI data UDP socket to port:" << ss.rawDataListenPort();
		}
		qfInfo() << "raw SI data UDP socket listenning on port:" << ss.rawDataListenPort();
	}

	if(ss.isListenSirxdData()) {
		m_sirxdDataServer = new QTcpServer(this);
		if (!m_sirxdDataServer->listen(QHostAddress::Any, ss.sirxdDataListenPort())) {
			qfError() << "MqttPunches: Sirxd TCP server cannot listen on port:" << ss.sirxdDataListenPort();
		}
		else {
			connect(m_sirxdDataServer, &QTcpServer::newConnection, [this](){
				//QTcpSocket *sock = m_sirxdDataServer->nextPendingConnection();
				//new MqttPunchesSirxdConnection(sock, m_sirxdDataServer);
			});
		}
		qfInfo() << "Sirxd TCP server listenning on port:" << ss.sirxdDataListenPort();
	}
	*/
}

}}
