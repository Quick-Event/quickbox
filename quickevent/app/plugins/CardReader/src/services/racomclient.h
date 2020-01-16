#pragma once

#include <Event/services/service.h>

class QUdpSocket;
class QTcpServer;
class QTcpSocket;

namespace siut { class DeviceDriver; }

namespace CardReader {
namespace services {

class RacomClientSettings : public Event::services::ServiceSettings
{
	using Super = Event::services::ServiceSettings;

	QF_VARIANTMAP_FIELD(bool, is, set, ListenRawData)
	QF_VARIANTMAP_FIELD(int, r, setR, awDataListenPort)
	QF_VARIANTMAP_FIELD(bool, is, set, ListenSirxdData)
	QF_VARIANTMAP_FIELD(int, s, setS, irxdDataListenPort)

public:
	RacomClientSettings(const QVariantMap &o = QVariantMap()) : Super(o) {}
};

class RacomClient : public Event::services::Service
{
	Q_OBJECT

	using Super = Service;
public:
	RacomClient(QObject *parent);

	void run() override;
	void stop() override;
	RacomClientSettings settings() const {return RacomClientSettings(m_settings);}

	static QString serviceName();
private:
	void onDbEventNotify(const QString &domain, int connection_id, const QVariant &data);
	void onRawSIDataUdpSocketReadyRead();
	qf::qmlwidgets::framework::DialogWidget *createDetailWidget() override;
	QUdpSocket *rawDataUdpSocket();
	void init();
private:
	QUdpSocket *m_rawSIDataUdpSocket = nullptr;
	QTcpServer *m_sirxdDataServer = nullptr;
	siut::DeviceDriver *m_siDriver = nullptr;
};

class RacomClientSirxdConnection : public QObject
{
	Q_OBJECT

	using Super = QObject;
public:
	RacomClientSirxdConnection(QTcpSocket *socket, QObject *parent);
private:
	void onReadyRead();
private:
	QTcpSocket *m_socket = nullptr;
	QByteArray m_data;
};

}}

