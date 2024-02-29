#pragma once

#include "shvnode.h"

namespace Event::services::shvapi {

class DotAppNode : public shvapi::ShvNode
{
	Q_OBJECT

	using Super = shvapi::ShvNode;
public:
	explicit DotAppNode(shv::iotqt::node::ShvNode *parent) : Super(".app", parent) {}
private:
	//shv::chainpack::RpcValue callMethodRq(const shv::chainpack::RpcRequest &rq) override;
	const std::vector<shv::chainpack::MetaMethod> &metaMethods() override;
	shv::chainpack::RpcValue callMethod(const StringViewList &shv_path, const std::string &method, const shv::chainpack::RpcValue &params, const shv::chainpack::RpcValue &user_id) override;
};

class EventNode : public shvapi::ShvNode
{
	Q_OBJECT

	using Super = shvapi::ShvNode;
public:
	explicit EventNode(shv::iotqt::node::ShvNode *parent) : Super("event", parent) {}
private:
	const std::vector<shv::chainpack::MetaMethod> &metaMethods() override;
	shv::chainpack::RpcValue callMethod(const StringViewList &shv_path, const std::string &method, const shv::chainpack::RpcValue &params, const shv::chainpack::RpcValue &user_id) override;
};

class StartListStarterNode : public shvapi::ShvNode
{
	Q_OBJECT

	using Super = shvapi::ShvNode;
public:
	explicit StartListStarterNode(int stage, shv::iotqt::node::ShvNode *parent)
		: Super("startliststarter", parent)
		, m_stage(stage)
	{}
private:
	const std::vector<shv::chainpack::MetaMethod> &metaMethods() override;
	shv::chainpack::RpcValue callMethod(const StringViewList &shv_path, const std::string &method, const shv::chainpack::RpcValue &params, const shv::chainpack::RpcValue &user_id) override;
private:
	int m_stage;
};

}
