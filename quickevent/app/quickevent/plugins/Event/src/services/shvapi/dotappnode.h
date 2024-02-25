#pragma once

#include "shvnode.h"

namespace Event::services::shvapi {

class DotAppNode : public shvapi::ShvNode
{
	Q_OBJECT

	using Super = shvapi::ShvNode;
public:
	explicit DotAppNode(shv::iotqt::node::ShvNode *parent);
private:
	//shv::chainpack::RpcValue callMethodRq(const shv::chainpack::RpcRequest &rq) override;
	shv::chainpack::RpcValue callMethod(const StringViewList &shv_path, const std::string &method, const shv::chainpack::RpcValue &params, const shv::chainpack::RpcValue &user_id) override;
	const std::vector<shv::chainpack::MetaMethod> &metaMethods() override;
};

}
