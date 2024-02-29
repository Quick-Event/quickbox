#include "sqlnode.h"
#include "rpcsqlresult.h"

#include <qf/core/exception.h>
#include <qf/core/sql/query.h>
#include <qf/core/log.h>

#include <shv/chainpack/rpc.h>
#include <shv/coreqt/rpc.h>

using namespace shv::chainpack;

namespace Event::services::shvapi {

SqlNode::SqlNode(shv::iotqt::node::ShvNode *parent)
	: Super("sql", parent)
{

}

static auto METH_EXEC_SQL = "execSql";

const std::vector<MetaMethod> &SqlNode::metaMethods()
{
	static std::vector<MetaMethod> meta_methods {
		{Rpc::METH_DIR, MetaMethod::Signature::RetParam, MetaMethod::Flag::None, Rpc::ROLE_BROWSE},
		{Rpc::METH_LS, MetaMethod::Signature::RetParam, MetaMethod::Flag::None, Rpc::ROLE_BROWSE},
		{METH_EXEC_SQL, MetaMethod::Signature::RetVoid, MetaMethod::Flag::None, Rpc::ROLE_WRITE},
	};
	return meta_methods;
}

RpcValue SqlNode::callMethod(const StringViewList &shv_path, const std::string &method, const shv::chainpack::RpcValue &params, const shv::chainpack::RpcValue &user_id)
{
	qfLogFuncFrame() << shv_path.join('/') << method;
	//eyascore::utils::UserId user_id = eyascore::utils::UserId::makeUserName(QString::fromStdString(rq.userId().toMap().value("userName").toString()));
	if(shv_path.empty()) {
		if(method == METH_EXEC_SQL) {
			qf::core::sql::Query q;
			QString qs = params.to<QString>();
			q.exec(qs, qf::core::Exception::Throw);
			auto res = RpcSqlResult::fromQuery(q);
			return res.toRpcValue();
		}
	}
	return Super::callMethod(shv_path, method, params, user_id);
}

}
