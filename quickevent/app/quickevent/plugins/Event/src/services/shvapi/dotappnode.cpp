#include "dotappnode.h"

#include <qf/core/exception.h>
#include <qf/core/log.h>

#include <shv/chainpack/rpc.h>

using namespace shv::chainpack;

namespace Event::services::shvapi {

DotAppNode::DotAppNode(shv::iotqt::node::ShvNode *parent)
	: Super(".app", parent)
{

}

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

}
