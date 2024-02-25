#include "rootnode.h"

#include <qf/core/exception.h>
#include <qf/core/log.h>

#include <shv/chainpack/rpc.h>

using namespace shv::chainpack;

namespace Event::services::shvapi {

RootNode::RootNode(QObject *parent)
	: Super(parent)
{

}

const std::vector<shv::chainpack::MetaMethod> &RootNode::metaMethods()
{
	static std::vector<MetaMethod> meta_methods {
		{Rpc::METH_DIR, MetaMethod::Signature::RetParam, MetaMethod::Flag::None, Rpc::ROLE_BROWSE},
		{Rpc::METH_LS, MetaMethod::Signature::RetParam, MetaMethod::Flag::None, Rpc::ROLE_BROWSE},
		{Rpc::METH_APP_NAME, MetaMethod::Signature::RetVoid, MetaMethod::Flag::IsGetter, Rpc::ROLE_BROWSE},
		//{Rpc::METH_DEVICE_TYPE, MetaMethod::Signature::RetVoid, MetaMethod::Flag::IsGetter, Rpc::ROLE_BROWSE},
		//{Rpc::METH_DEVICE_ID, MetaMethod::Signature::RetVoid, MetaMethod::Flag::IsGetter, Rpc::ROLE_READ},
		//{M_APP_VERSION, MetaMethod::Signature::RetVoid, MetaMethod::Flag::IsGetter, Rpc::ROLE_READ},
		//{M_GIT_COMMIT, MetaMethod::Signature::RetVoid, MetaMethod::Flag::IsGetter, Rpc::ROLE_READ},
		//{METH_DEPOT_MODEL_DESTROY_ROUTE, MetaMethod::Signature::RetParam, MetaMethod::Flag::None, Rpc::ROLE_WRITE},
	};
	return meta_methods;
}

size_t RootNode::methodCount(const StringViewList &shv_path)
{
	if(shv_path.empty()) {
		return metaMethods().size();
	}
	return Super::methodCount(shv_path);
}

const MetaMethod *RootNode::metaMethod(const StringViewList &shv_path, size_t ix)
{
	if(shv_path.empty()) {
		if(metaMethods().size() <= ix)
			QF_EXCEPTION("Invalid method index: " + QString::number(ix) + " of: " + QString::number(metaMethods().size()));
		return &(metaMethods()[ix]);
	}
	return Super::metaMethod(shv_path, ix);
}

RpcValue RootNode::callMethod(const StringViewList &shv_path, const std::string &method, const shv::chainpack::RpcValue &params, const shv::chainpack::RpcValue &user_id)
{
	qfLogFuncFrame() << shv_path.join('/') << method;
	//eyascore::utils::UserId user_id = eyascore::utils::UserId::makeUserName(QString::fromStdString(rq.userId().toMap().value("userName").toString()));
	if(shv_path.empty()) {
		if(method == Rpc::METH_APP_NAME) {
			return "QuickEvent";
		}
	}
	return Super::callMethod(shv_path, method, params, user_id);
}

}
