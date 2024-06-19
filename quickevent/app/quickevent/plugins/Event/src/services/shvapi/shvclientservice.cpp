#include "shvclientservice.h"
#include "shvclientservicewidget.h"
#include "nodes.h"
#include "sqlapinode.h"

#include "../../eventplugin.h"
#include "../../../../Runs/src/runsplugin.h"

#include <qf/qmlwidgets/framework/mainwindow.h>
#include <qf/core/log.h>
#include <qf/core/sql/query.h>

#include <shv/iotqt/rpc/deviceconnection.h>
#include <shv/iotqt/node/shvnode.h>
#include <shv/coreqt/rpc.h>

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QHttpPart>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QSettings>
#include <QStandardPaths>
#include <QTextStream>
#include <QTimer>

using namespace qf::core;
using namespace qf::qmlwidgets;
using namespace qf::qmlwidgets::dialogs;
using namespace qf::core::sql;
using namespace shv::iotqt::rpc;
using namespace shv::chainpack;
using qf::qmlwidgets::framework::getPlugin;
using Event::EventPlugin;
using Runs::RunsPlugin;

#define logD() nCDebug("")
#define logM() nCMessage("")
#define logI() nCInfo("")
#define logW() nCWarning("")
#define logE() nCError("")

namespace Event::services::shvapi {

ShvClientService::ShvClientService(QObject *parent)
	: Super(ShvClientService::serviceName(), parent)
	, m_rpcConnection(nullptr)
	, m_rootNode(new shv::iotqt::node::ShvRootNode(this))
{
	auto *event_plugin = getPlugin<EventPlugin>();

	new DotAppNode(m_rootNode);
	connect(event_plugin, &EventPlugin::eventOpenChanged, this, [this](bool is_open) {
		if (is_open) {
			new SqlApiNode(m_rootNode);
			auto *event = new EventNode(m_rootNode);
			auto *current_stage = new shv::iotqt::node::ShvNode("currentStage", event);
			new CurrentStageConfigNode(current_stage);
			//new CurrentStageStartListNode(current_stage);
			new CurrentStageRunsNode(current_stage);
			new CurrentStageClassesNode(current_stage);
		}
		else {
			qDeleteAll(m_rootNode->findChildren<EventNode*>());
		}
	});

	connect(event_plugin, &Event::EventPlugin::dbEventNotify, this, &ShvClientService::onDbEventNotify, Qt::QueuedConnection);
	connect(m_rootNode, &shv::iotqt::node::ShvNode::sendRpcMessage, this, &ShvClientService::sendRpcMessage);
}

QString ShvClientService::serviceName()
{
	return QStringLiteral("ShvApi");
}

void ShvClientService::run() {
	QF_SAFE_DELETE(m_rpcConnection);
	auto ss = settings();
	m_rpcConnection = new DeviceConnection(this);
	connect(m_rpcConnection, &DeviceConnection::rpcMessageReceived, this, &ShvClientService::onRpcMessageReceived);
	connect(m_rpcConnection, &DeviceConnection::brokerConnectedChanged, this, [this](bool is_connected) {
		if (is_connected) {
			setStatus(Status::Running);
		}
		else {
			logE() << "Connect to" << m_rpcConnection->connectionUrl().toString() << "error.";
		}
	});
	m_rpcConnection->setConnectionString(ss.shvConnectionUrl());
	RpcValue::Map opts;
	RpcValue::Map device;
	device["mountPoint"] = ss.eventPath().toStdString();
	opts["device"] = device;
	m_rpcConnection->setConnectionOptions(opts);
	m_rpcConnection->open();
}

void ShvClientService::stop() {
	QF_SAFE_DELETE(m_rpcConnection);
	Super::stop();
}

qf::qmlwidgets::framework::DialogWidget *ShvClientService::createDetailWidget()
{
	auto *w = new ShvClientServiceWidget();
	return w;
}

void ShvClientService::onRpcMessageReceived(const shv::chainpack::RpcMessage &msg)
{
	qfDebug() << "client RPC request:"  << msg.toCpon();
	if(msg.isSignal()) {
		return;
	}
	if(msg.isRequest()) {
		RpcRequest rq(msg);
		auto api_key = settings().apiKey();
		auto user_id = rq.userId().asString();
		auto correct_user_id = QStringLiteral("api_key=%1").arg(api_key).toStdString();
		if (user_id == correct_user_id) {
			RpcResponse resp = RpcResponse::forRequest(rq);
			resp.setError(RpcResponse::Error("Invalid API key", RpcResponse::Error::MethodNotFound));
			m_rootNode->emitSendRpcMessage(resp);
		}
		else {
			m_rootNode->handleRpcRequest(rq);
		}
		return;
	}
	if(msg.isResponse()) {
		//shvInfo() << "==> NTF method:" << method.toString();
		return;
	}
}

void ShvClientService::sendRpcMessage(const shv::chainpack::RpcMessage &rpc_msg)
{
	if(m_rpcConnection && m_rpcConnection->isBrokerConnected()) {
		m_rpcConnection->sendRpcMessage(rpc_msg);
	}
}

void ShvClientService::loadSettings()
{
	Super::loadSettings();
	auto ss = settings();
	auto *event_plugin = getPlugin<EventPlugin>();
	if (ss.eventPath().isEmpty()) {
		ss.setEventPath("test/qe/" + event_plugin->shvApiEventId());
	}
	if (ss.apiKey().isEmpty()) {
		ss.setApiKey(event_plugin->createShvApiKey());
	}
	m_settings = ss;
}

void ShvClientService::onDbEventNotify(const QString &domain, int connection_id, const QVariant &data)
{
	if (status() != Status::Running)
		return;
	Q_UNUSED(connection_id)
	Q_UNUSED(data)
	if(domain == QLatin1String(Event::EventPlugin::DBEVENT_CARD_PROCESSED_AND_ASSIGNED)) {
		//auto checked_card = quickevent::core::si::CheckedCard(data.toMap());
		//int competitor_id = getPlugin<RunsPlugin>()->competitorForRun(checked_card.runId());
		//onCompetitorChanged(competitor_id);
	}
	else if(domain == QLatin1String(Event::EventPlugin::DBEVENT_COMPETITOR_EDITED)) {
		//int competitor_id = data.toInt();
		//onCompetitorChanged(competitor_id);
	}
	else if (domain == Event::EventPlugin::DBEVENT_RUN_CHANGED) {
		if (auto *node = m_rootNode->findChild<CurrentStageRunsNode*>(); node) {
			node->sendRunChangedSignal(data);
		}
	}
}

}
