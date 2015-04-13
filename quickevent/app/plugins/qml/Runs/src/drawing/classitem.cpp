#include "classitem.h"

#include <qf/core/sql/query.h>

using namespace drawing;

ClassData::ClassData(const qf::core::sql::Query &q)
{
	for(auto s : {"id", "courseId", "startSlotIndex", "startSlotPosition", "startTimeMin", "startIntervalMin", "vacantsBefore", "vacantsBefore", "vacantEvery", "vacantsAfter"}) {
		insert(s, q.value(s));
	}
}

ClassItem::ClassItem(QGraphicsItem *parent)
	: Super(parent)
{

}

const ClassData &ClassItem::data() const
{
	return m_data;
}

void ClassItem::setData(const ClassData &data)
{
	m_data = data;
}






