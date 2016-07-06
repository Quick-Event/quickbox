#include "punchrecord.h"

#include <siut/simessage.h>

#include <qf/core/sql/query.h>

namespace quickevent {
namespace si {

const QString PunchRecord::MARKING_RACE = QStringLiteral("race");
const QString PunchRecord::MARKING_ENTRIES = QStringLiteral("entries");

PunchRecord::PunchRecord(const SIMessageTransmitPunch &rec)
	: Super(rec.toVariantMap())
{
}

PunchRecord::PunchRecord(const qf::core::sql::Query &q)
	: Super(q.values())
{
}

} // namespace si
} // namespace quickevent
