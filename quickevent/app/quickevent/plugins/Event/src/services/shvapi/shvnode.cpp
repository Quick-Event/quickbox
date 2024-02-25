#include "shvnode.h"

#include <qf/core/exception.h>
#include <qf/core/log.h>

#include <shv/chainpack/rpc.h>

using namespace shv::chainpack;

namespace Event::services::shvapi {

ShvNode::ShvNode(const std::string &name, shv::iotqt::node::ShvNode *parent)
	: Super(name, parent)
{

}

size_t ShvNode::methodCount(const StringViewList &shv_path)
{
	if(shv_path.empty()) {
		return metaMethods().size();
	}
	return Super::methodCount(shv_path);
}

const MetaMethod *ShvNode::metaMethod(const StringViewList &shv_path, size_t ix)
{
	if(shv_path.empty()) {
		if(metaMethods().size() <= ix)
			QF_EXCEPTION("Invalid method index: " + QString::number(ix) + " of: " + QString::number(metaMethods().size()));
		return &(metaMethods()[ix]);
	}
	return Super::metaMethod(shv_path, ix);
}

}
