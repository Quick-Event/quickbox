#include "rootnode.h"
#include "dotappnode.h"

#include <qf/core/exception.h>
#include <qf/core/log.h>

#include <shv/chainpack/rpc.h>

using namespace shv::chainpack;

namespace Event::services::shvapi {

RootNode::RootNode(QObject *parent)
	: Super(parent)
{
	new DotAppNode(this);
}

const std::vector<shv::chainpack::MetaMethod> &RootNode::metaMethods()
{
	static std::vector<MetaMethod> meta_methods {
		{Rpc::METH_DIR, MetaMethod::Signature::RetParam, MetaMethod::Flag::None, Rpc::ROLE_BROWSE},
		{Rpc::METH_LS, MetaMethod::Signature::RetParam, MetaMethod::Flag::None, Rpc::ROLE_BROWSE},
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
	return Super::callMethod(shv_path, method, params, user_id);
}

}
