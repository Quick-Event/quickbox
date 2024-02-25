#pragma once

#include <shv/iotqt/node/shvnode.h>

namespace Event::services::shvapi {

class RootNode : public shv::iotqt::node::ShvRootNode
{
	Q_OBJECT

	using Super = shv::iotqt::node::ShvRootNode;
public:
	explicit RootNode(QObject *parent);

	size_t methodCount(const StringViewList &shv_path) override;
	const shv::chainpack::MetaMethod* metaMethod(const StringViewList &shv_path, size_t ix) override;

private:
	//shv::chainpack::RpcValue callMethodRq(const shv::chainpack::RpcRequest &rq) override;
	shv::chainpack::RpcValue callMethod(const StringViewList &shv_path, const std::string &method, const shv::chainpack::RpcValue &params, const shv::chainpack::RpcValue &user_id) override;
	static const std::vector<shv::chainpack::MetaMethod> &metaMethods();
};

}
