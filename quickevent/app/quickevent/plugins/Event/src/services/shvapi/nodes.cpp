#include "nodes.h"
#include "rpcsqlresult.h"

#include "../../eventplugin.h"

#include <qf/qmlwidgets/framework/mainwindow.h>
#include <qf/core/exception.h>
#include <qf/core/log.h>
#include <qf/core/sql/query.h>

#include <shv/chainpack/rpc.h>
#include <shv/coreqt/rpc.h>

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
		{Rpc::METH_DIR, MetaMethod::Signature::RetParam, MetaMethod::Flag::None, Rpc::ROLE_BROWSE},
		{Rpc::METH_LS, MetaMethod::Signature::RetParam, MetaMethod::Flag::None, Rpc::ROLE_BROWSE},
		{Rpc::METH_PING, MetaMethod::Signature::RetVoid, MetaMethod::Flag::None, Rpc::ROLE_BROWSE},
		{METH_NAME, MetaMethod::Signature::RetVoid, MetaMethod::Flag::IsGetter, Rpc::ROLE_BROWSE},
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
static auto METH_CURRENT_STAGE = "currentStage";
static auto SIG_RUN_CHANGED = "runChanged";

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
		{Rpc::METH_DIR, MetaMethod::Signature::RetParam, MetaMethod::Flag::None, Rpc::ROLE_BROWSE},
		{Rpc::METH_LS, MetaMethod::Signature::RetParam, MetaMethod::Flag::None, Rpc::ROLE_BROWSE},
		{METH_CURRENT_STAGE, MetaMethod::Signature::RetVoid, MetaMethod::Flag::IsGetter, Rpc::ROLE_READ},
		{SIG_RUN_CHANGED, MetaMethod::Signature::VoidParam, MetaMethod::Flag::IsSignal, Rpc::ROLE_READ},
	};
	return meta_methods;
}

RpcValue EventNode::callMethod(const StringViewList &shv_path, const std::string &method, const shv::chainpack::RpcValue &params, const shv::chainpack::RpcValue &user_id)
{
	qfLogFuncFrame() << shv_path.join('/') << method;
	if(shv_path.empty()) {
		if(method == METH_CURRENT_STAGE) {
			return getPlugin<EventPlugin>()->currentStageId();
		}
	}
	return Super::callMethod(shv_path, method, params, user_id);
}

//=========================================================
// StartListNode
//=========================================================
static auto METH_TABLE = "table";
static auto METH_RECORD = "record";
static auto METH_SET_RECORD = "setRecord";
static auto SIG_REC_CHNG = "recchng";

void SqlViewNode::setQueryBuilder(const qf::core::sql::QueryBuilder &qb)
{
	m_queryBuilder = qb;
}

const std::vector<MetaMethod> &SqlViewNode::metaMethods()
{
	static std::vector<MetaMethod> meta_methods {
		{Rpc::METH_DIR, MetaMethod::Signature::RetParam, MetaMethod::Flag::None, Rpc::ROLE_BROWSE},
		{Rpc::METH_LS, MetaMethod::Signature::RetParam, MetaMethod::Flag::None, Rpc::ROLE_BROWSE},
		{METH_TABLE, MetaMethod::Signature::RetVoid, MetaMethod::Flag::None, Rpc::ROLE_READ},
		{METH_RECORD, MetaMethod::Signature::RetParam, MetaMethod::Flag::None, Rpc::ROLE_READ},
		{METH_SET_RECORD, MetaMethod::Signature::VoidParam, MetaMethod::Flag::None, Rpc::ROLE_WRITE},
		{SIG_REC_CHNG, MetaMethod::Signature::VoidParam, MetaMethod::Flag::IsSignal, Rpc::ROLE_READ},
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
			auto qb = m_queryBuilder;
			if (!where.isEmpty()) {
				qb.where(where);
			}
			qf::core::sql::Query q;
			QString qs = qb.toString();
			q.exec(qs, qf::core::Exception::Throw);
			auto res = RpcSqlResult::fromQuery(q);
			return res.toRpcValue();
		}
		if(method == METH_RECORD) {
			auto id = params.toInt();
			auto qb = m_queryBuilder;
			qb.where("runs.id = " + QString::number(id));
			qf::core::sql::Query q;
			QString qs = qb.toString();
			qfDebug() << qs;
			q.exec(qs, qf::core::Exception::Throw);
			if (q.next()) {
				return shvapi::recordToMap(q.record());
			}
			else {
				return RpcValue::Map{};
			}
		}
	}
	return Super::callMethod(shv_path, method, params, user_id);
}

}
