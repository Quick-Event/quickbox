#pragma once

#include "../service.h"

class QTimer;
class QNetworkAccessManager;

namespace shv::chainpack { class RpcMessage; }
namespace shv::iotqt::rpc { class DeviceConnection; }
namespace shv::iotqt::node { class ShvRootNode; }

namespace Event::services::shvapi {

class RootNode;

class ShvClientServiceSettings : public ServiceSettings
{
	using Super = ServiceSettings;

	QF_VARIANTMAP_FIELD(QString, s, setS, hvConnectionUrl)
	QF_VARIANTMAP_FIELD(QString, e, setE, ventPath)
	QF_VARIANTMAP_FIELD(QString, a, setA, piKey)
public:
	ShvClientServiceSettings(const QVariantMap &o = QVariantMap()) : Super(o) {}
};

class ShvClientService : public Service
{
	Q_OBJECT

	using Super = Service;
public:
	ShvClientService(QObject *parent);

	void run() override;
	void stop() override;
	ShvClientServiceSettings settings() const {return ShvClientServiceSettings(m_settings);}

	static QString serviceName();

	void loadSettings() override;
	void onDbEventNotify(const QString &domain, int connection_id, const QVariant &data);
private:
	qf::qmlwidgets::framework::DialogWidget *createDetailWidget() override;
	void onRpcMessageReceived(const shv::chainpack::RpcMessage &msg);
	void sendRpcMessage(const shv::chainpack::RpcMessage &rpc_msg);
private:
	shv::iotqt::rpc::DeviceConnection *m_rpcConnection;
	shv::iotqt::node::ShvRootNode *m_rootNode;
};

}
