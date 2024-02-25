#pragma once

#include "../service.h"

class QTimer;
class QNetworkAccessManager;

namespace shv::chainpack { class RpcMessage; }
namespace shv::iotqt::rpc { class DeviceConnection; }

namespace Event::services::shvapi {

class RootNode;

class ClientSettings : public ServiceSettings
{
	using Super = ServiceSettings;

	QF_VARIANTMAP_FIELD(QString, s, setS, hvConnectionUrl)
public:
	ClientSettings(const QVariantMap &o = QVariantMap()) : Super(o) {}
};

class Client : public Service
{
	Q_OBJECT

	using Super = Service;
public:
	Client(QObject *parent);

	void run() override;
	void stop() override;
	ClientSettings settings() const {return ClientSettings(m_settings);}

	static QString serviceName();

	void loadSettings() override;
	void onDbEventNotify(const QString &domain, int connection_id, const QVariant &data);
private:
	qf::qmlwidgets::framework::DialogWidget *createDetailWidget() override;
	void onRpcMessageReceived(const shv::chainpack::RpcMessage &msg);
	void sendRpcMessage(const shv::chainpack::RpcMessage &rpc_msg);
private:
	shv::iotqt::rpc::DeviceConnection *m_rpcConnection;
	RootNode *m_rootNode;
};

}
