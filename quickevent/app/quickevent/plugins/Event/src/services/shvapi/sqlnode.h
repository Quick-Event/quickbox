#pragma once

#include "shvnode.h"

namespace Event::services::shvapi {

class SqlNode : public ShvNode
{
	Q_OBJECT

	using Super = ShvNode;
public:
	explicit SqlNode(shv::iotqt::node::ShvNode *parent);

protected:
	const std::vector<shv::chainpack::MetaMethod> &metaMethods() override;
	shv::chainpack::RpcValue callMethod(const StringViewList &shv_path, const std::string &method, const shv::chainpack::RpcValue &params, const shv::chainpack::RpcValue &user_id) override;
};

}
