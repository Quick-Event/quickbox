#pragma once

#include "service.h"

class QUdpSocket;

namespace services {

class RacomClientSettings : public ServiceSettings
{
	using Super = ServiceSettings;

	QF_VARIANTMAP_FIELD(int, p, setP, ort)

public:
	RacomClientSettings(const QVariantMap &o = QVariantMap()) : Super(o) {}
};

class RacomClient : public Service
{
	Q_OBJECT

	using Super = Service;
public:
	RacomClient(QObject *parent);

	//void run() override;
	//void stop() override;
	RacomClientSettings settings() const {return RacomClientSettings(m_settings);}

	static QString serviceName();
private:
	void onDbEventNotify(const QString &domain, int connection_id, const QVariant &data);
	void onUdpSocketReadyRead();
	qf::qmlwidgets::framework::DialogWidget *createDetailWidget() override;
	QUdpSocket *udpSocket();
private:
	QUdpSocket *m_udpSocket = nullptr;
};

} // namespace services

