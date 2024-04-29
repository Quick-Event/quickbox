#include "nodes.h"

#include "sqlnode.h"
#include "../../eventplugin.h"

#include <qf/qmlwidgets/framework/mainwindow.h>
#include <qf/core/exception.h>
#include <qf/core/log.h>
#include <qf/core/sql/query.h>

#include <shv/chainpack/rpc.h>
#include <shv/coreqt/rpc.h>
#include <shv/coreqt/data/rpcsqlresult.h>

#include <QSqlField>

using namespace shv::chainpack;

using qf::qmlwidgets::framework::getPlugin;

namespace Event::services::shvapi {

//=========================================================
// DotAppNode
//=========================================================
static auto METH_NAME = "name";

const std::vector<shv::chainpack::MetaMethod> &DotAppNode::metaMethods()
{
	static std::vector<MetaMethod> meta_methods {
		methods::DIR,
		methods::LS,
		{Rpc::METH_PING, MetaMethod::Flag::None, {}, "RpcValue", AccessLevel::Browse},
		{METH_NAME, MetaMethod::Flag::IsGetter, {}, "RpcValue", AccessLevel::Browse},
	};
	return meta_methods;
}

RpcValue DotAppNode::callMethod(const StringViewList &shv_path, const std::string &method, const shv::chainpack::RpcValue &params, const shv::chainpack::RpcValue &user_id)
{
	qfLogFuncFrame() << shv_path.join('/') << method;
	//eyascore::utils::UserId user_id = eyascore::utils::UserId::makeUserName(QString::fromStdString(rq.userId().toMap().value("userName").toString()));
	if(shv_path.empty()) {
		if(method == Rpc::METH_PING) {
			return nullptr;
		}
		if(method == METH_NAME) {
			return "QuickEvent";
		}
	}
	return Super::callMethod(shv_path, method, params, user_id);
}

//=========================================================
// EventNode
//=========================================================
static const auto METH_CURRENT_STAGE = "currentStage";
static const auto METH_EVENT_CONFIG = "eventConfig";
static const auto METH_RUN_CHANGED = "runChanged";
static const auto SIG_RUN_CHANGED = "runChanged";

EventNode::EventNode(shv::iotqt::node::ShvNode *parent)
: Super("event", parent)
{
	auto *event_plugin = getPlugin<EventPlugin>();
	connect(event_plugin, &EventPlugin::dbEvent, this, [this](const QString &domain, const QVariant &payload) {
		if (domain == Event::EventPlugin::DBEVENT_RUN_CHANGED) {
			auto param = shv::coreqt::rpc::qVariantToRpcValue(payload);
			Q_ASSERT(param.isList());
			Q_ASSERT(param.asList().value(0).toInt() > 0);
			RpcSignal sig;
			sig.setShvPath(shvPath());
			sig.setMethod(SIG_RUN_CHANGED);
			sig.setParams(param);
			qfDebug() << "emit:" << sig.toPrettyString();
			emitSendRpcMessage(sig);
		}
	});
}

const std::vector<MetaMethod> &EventNode::metaMethods()
{
	static std::vector<MetaMethod> meta_methods {
		methods::DIR,
		methods::LS,
		{METH_NAME, MetaMethod::Flag::IsGetter, {}, "RpcValue", AccessLevel::Read},
		{METH_CURRENT_STAGE, MetaMethod::Flag::IsGetter, {}, "RpcValue", AccessLevel::Read},
		{METH_EVENT_CONFIG, MetaMethod::Flag::IsGetter, {}, "RpcValue", AccessLevel::Read},
		{METH_RUN_CHANGED, MetaMethod::Flag::None, {}, {}, AccessLevel::Read, {{SIG_RUN_CHANGED, "Map"}}},
	};
	return meta_methods;
}

RpcValue EventNode::callMethod(const StringViewList &shv_path, const std::string &method, const shv::chainpack::RpcValue &params, const shv::chainpack::RpcValue &user_id)
{
	qfLogFuncFrame() << shv_path.join('/') << method;
	if(shv_path.empty()) {
		if(method == METH_NAME) {
			return getPlugin<EventPlugin>()->eventConfig()->eventName().toStdString();
		}
		if(method == METH_CURRENT_STAGE) {
			return getPlugin<EventPlugin>()->currentStageId();
		}
		if(method == METH_EVENT_CONFIG) {
			auto cfg = getPlugin<EventPlugin>()->eventConfig();
			return shv::coreqt::rpc::qVariantToRpcValue(cfg->values());
		}
	}
	return Super::callMethod(shv_path, method, params, user_id);
}

//=========================================================
// StartListNode
//=========================================================
static const auto METH_TABLE = "table";
static const auto METH_RECORD = "record";
//static const auto METH_SET_RECORD = "setRecord";
//static const auto SIG_REC_CHNG = "recchng";

void SqlViewNode::setQueryBuilder(const qf::core::sql::QueryBuilder &qb)
{
	m_queryBuilder = qb;
}

qf::core::sql::QueryBuilder SqlViewNode::effectiveQueryBuilder()
{
	return m_queryBuilder;
}

const std::vector<MetaMethod> &SqlViewNode::metaMethods()
{
	static std::vector<MetaMethod> meta_methods {
		methods::DIR,
		methods::LS,
		{METH_TABLE, MetaMethod::Flag::None, {}, "RpcValue", AccessLevel::Read},
		{METH_RECORD, MetaMethod::Flag::None, "RpcValue", "RpcValue", AccessLevel::Read },
		//{METH_SET_RECORD, MetaMethod::Flag::None, "RpcValue", {}, AccessLevel::Write},
	};
	return meta_methods;
}

RpcValue SqlViewNode::callMethod(const StringViewList &shv_path, const std::string &method, const shv::chainpack::RpcValue &params, const shv::chainpack::RpcValue &user_id)
{
	qfLogFuncFrame() << shv_path.join('/') << method;
	//eyascore::utils::UserId user_id = eyascore::utils::UserId::makeUserName(QString::fromStdString(rq.userId().toMap().value("userName").toString()));
	if(shv_path.empty()) {
		if(method == METH_TABLE) {
			const auto &m = params.asMap();
			auto where = m.value("where").to<QString>();
			auto qb = effectiveQueryBuilder();
			if (!where.isEmpty()) {
				qb.where(where);
			}
			qf::core::sql::Query q;
			QString qs = qb.toString();
			q.exec(qs, qf::core::Exception::Throw);
			auto res = SqlNode::rpcSqlResultFromQuery(q);
			return res.toRpcValue();
		}
		if(method == METH_RECORD) {
			auto id = params.toInt();
			auto qb = effectiveQueryBuilder();
			qb.where("runs.id = " + QString::number(id));
			qf::core::sql::Query q;
			QString qs = qb.toString();
			qfDebug() << qs;
			q.exec(qs, qf::core::Exception::Throw);
			if (q.next()) {
				return SqlNode::recordToMap(q.record());
			}
			return RpcValue::Map{};
		}
	}
	return Super::callMethod(shv_path, method, params, user_id);
}

//=========================================================
// CurrentStageSqlViewNode
//=========================================================
qf::core::sql::QueryBuilder CurrentStageSqlViewNode::effectiveQueryBuilder()
{
	auto qb = Super::effectiveQueryBuilder();
	auto *event_plugin = getPlugin<EventPlugin>();
	qb.where(QStringLiteral("runs.stageId=%1").arg(event_plugin->currentStageId()));
	return qb;
}

}
