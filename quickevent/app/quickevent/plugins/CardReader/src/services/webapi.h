#pragma once

#include <plugins/Event/src/services/service.h>

#include <siut/sidevicedriver.h>

#include <QTcpServer>

class QTcpSocket;

namespace CardReader {
namespace services {

class WebApiSettings : public Event::services::ServiceSettings
{
	using Super = Event::services::ServiceSettings;

	QF_VARIANTMAP_FIELD(int, t, setT, cpListenPort)

public:
	WebApiSettings(const QVariantMap &o = QVariantMap()) : Super(o) {}
};

class WebApi : public Event::services::Service
{
	Q_OBJECT

	using Super = Service;
public:
	WebApi(QObject *parent);

	void run() override;
	void stop() override;
	WebApiSettings settings() const {return WebApiSettings(m_settings);}

	static QString serviceName();
private:
	qf::qmlwidgets::framework::DialogWidget *createDetailWidget() override;
	void init();
	void onTcpReadoutReceived(const QVariant &data);
	void onNewConnection();
	void onReadyRead();
private:
	std::unique_ptr<QTcpServer> m_tcpServer;
	std::unordered_map<QTcpSocket*, std::unique_ptr<QByteArray>> m_tcpClients;
	std::unique_ptr<siut::DeviceDriver> m_siDriver;
};

}}
