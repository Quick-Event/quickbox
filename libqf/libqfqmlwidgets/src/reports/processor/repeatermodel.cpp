#include "repeatermodel.h"

#include <qf/core/log.h>
#include <qf/core/assert.h>
#include <qf/core/string.h>

namespace qfc = qf::core;
namespace qfu = qf::core::utils;

using namespace qf::qmlwidgets::reports;

//=======================================================
//                   RepeaterModel
//=======================================================
RepeaterModel::RepeaterModel(QObject *parent) :
	Super(parent)
{
}

RepeaterModel* RepeaterModel::createFromData(const QVariant &data, QObject *parent)
{
	RepeaterModel *ret = nullptr;
	if(data.userType() == qMetaTypeId<qfu::TreeTable>()) {
		RepeaterModelTreeTable *m = new RepeaterModelTreeTable(parent);
		qfu::TreeTable tt = data.value<qfu::TreeTable>();
		m->setTreeTable(tt);
		ret = m;
	}
	return ret;
}

//=======================================================
//                   RepeaterModelTreeTable
//=======================================================
RepeaterModelTreeTable::RepeaterModelTreeTable(QObject *parent)
	: Super(parent)
{

}
