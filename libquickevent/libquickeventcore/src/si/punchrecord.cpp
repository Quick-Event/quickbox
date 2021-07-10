#include "punchrecord.h"

#include <siut/sipunch.h>

#include <qf/core/sql/query.h>

#include <QJsonDocument>

namespace quickevent {
namespace core {
namespace si {

PunchRecord::PunchRecord(const qf::core::sql::Query &q)
	: Super(q.values())
{
}

QString PunchRecord::toString() const
{
	QJsonDocument jsd = QJsonDocument::fromVariant(*this);
	return QString::fromUtf8(jsd.toJson(QJsonDocument::Compact));
}

}}}
