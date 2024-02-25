#include "nodes.h"

#include "../../eventplugin.h"

#include <qf/qmlwidgets/framework/mainwindow.h>
#include <qf/core/exception.h>
#include <qf/core/log.h>

#include <shv/chainpack/rpc.h>

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

const std::vector<MetaMethod> &EventNode::metaMethods()
{
	static std::vector<MetaMethod> meta_methods {
		{Rpc::METH_DIR, MetaMethod::Signature::RetParam, MetaMethod::Flag::None, Rpc::ROLE_BROWSE},
		{Rpc::METH_LS, MetaMethod::Signature::RetParam, MetaMethod::Flag::None, Rpc::ROLE_BROWSE},
		{METH_CURRENT_STAGE, MetaMethod::Signature::RetVoid, MetaMethod::Flag::IsGetter, Rpc::ROLE_READ},
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

const std::vector<MetaMethod> &RunNode::metaMethods()
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

RpcValue RunNode::callMethod(const StringViewList &shv_path, const std::string &method, const shv::chainpack::RpcValue &params, const shv::chainpack::RpcValue &user_id)
{
	qfLogFuncFrame() << shv_path.join('/') << method;
	//eyascore::utils::UserId user_id = eyascore::utils::UserId::makeUserName(QString::fromStdString(rq.userId().toMap().value("userName").toString()));
	if(shv_path.empty()) {
		if(method == METH_TABLE) {
		}
	}
	return Super::callMethod(shv_path, method, params, user_id);
}

}
