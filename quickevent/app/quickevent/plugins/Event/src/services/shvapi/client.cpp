#include "client.h"
#include "clientwidget.h"
#include "nodes.h"
#include "sqlnode.h"

#include "../../eventplugin.h"

#include <qf/qmlwidgets/framework/mainwindow.h>
#include <qf/core/log.h>
#include <plugins/Runs/src/runsplugin.h>
#include <qf/core/sql/query.h>

#include <shv/iotqt/rpc/deviceconnection.h>
#include <shv/iotqt/node/shvnode.h>

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

Client::Client(QObject *parent)
	: Super(Client::serviceName(), parent)
	, m_rpcConnection(nullptr)
	, m_rootNode(new shv::iotqt::node::ShvRootNode(this))
{
	auto *event_plugin = getPlugin<EventPlugin>();

	new DotAppNode(m_rootNode);
	connect(event_plugin, &EventPlugin::eventOpenChanged, this, [this](bool is_open) {
		if (is_open) {
			auto *event_plugin = getPlugin<EventPlugin>();
			auto *event = new EventNode(m_rootNode);
			auto *stage = new shv::iotqt::node::ShvNode("stage", event);
			for (auto i = 0; i < event_plugin->stageCount(); i++) {
				auto *nd = new shv::iotqt::node::ShvNode(std::to_string(i + 1), stage);
				auto *runs = new SqlViewNode("runs", nd);
				auto qb = getPlugin<RunsPlugin>()->runsQuery(i + 1);
				runs->setQueryBuilder(qb);
			}
			new SqlNode(m_rootNode);
		}
		else {
			qDeleteAll(m_rootNode->findChildren<EventNode*>());
		}
	});

	connect(event_plugin, &Event::EventPlugin::dbEventNotify, this, &Client::onDbEventNotify, Qt::QueuedConnection);
	connect(m_rootNode, &shv::iotqt::node::ShvNode::sendRpcMessage, this, &Client::sendRpcMessage);
}

QString Client::serviceName()
{
	return QStringLiteral("ShvApi");
}

void Client::run() {
	QF_SAFE_DELETE(m_rpcConnection);
	auto ss = settings();
	m_rpcConnection = new DeviceConnection(this);
	connect(m_rpcConnection, &DeviceConnection::rpcMessageReceived, this, &Client::onRpcMessageReceived);
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
	device["mountPoint"] = "test/QE";
	opts["device"] = device;
	m_rpcConnection->setConnectionOptions(opts);
	m_rpcConnection->open();
}

void Client::stop() {
	QF_SAFE_DELETE(m_rpcConnection);
	Super::stop();
}

qf::qmlwidgets::framework::DialogWidget *Client::createDetailWidget()
{
	auto *w = new ClientWidget();
	return w;
}

void Client::onRpcMessageReceived(const shv::chainpack::RpcMessage &msg)
{
	qfDebug() << "client RPC request:"  << msg.toCpon();
	if(msg.isSignal()) {
		return;
	}
	if(msg.isRequest()) {
		RpcRequest rq(msg);
		m_rootNode->handleRpcRequest(rq);
	}
	else if(msg.isResponse()) {
		//shvInfo() << "==> NTF method:" << method.toString();
		return;
	}
}

void Client::sendRpcMessage(const shv::chainpack::RpcMessage &rpc_msg)
{
	if(m_rpcConnection && m_rpcConnection->isBrokerConnected()) {
		m_rpcConnection->sendRpcMessage(rpc_msg);
	}
}

void Client::loadSettings()
{
	Super::loadSettings();
}

void Client::onDbEventNotify(const QString &domain, int connection_id, const QVariant &data)
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
	if(domain == QLatin1String(Event::EventPlugin::DBEVENT_COMPETITOR_EDITED)) {
		//int competitor_id = data.toInt();
		//onCompetitorChanged(competitor_id);
	}
}

}