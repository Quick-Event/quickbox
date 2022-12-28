#include "resultstatus.h"
#include "og/timems.h"
#include <QCoreApplication>

namespace quickevent {
namespace core {

ResultStatus::ResultStatus(qf::core::sql::Query &q)
{
	fillFromQuery(q);
}

ResultStatus::ResultStatus(const qf::core::utils::TreeTableRow &ttr)
{
	fillFromTreeTableRow(ttr);
}

QString ResultStatus::dbRunsColumnList()
{
	return QStringLiteral(" disqualified, disqualifiedByOrganizer, notCompeting, misPunch, notStart, notFinish, overTime");
}

void ResultStatus::fillFromQuery(qf::core::sql::Query &q)
{
	m_disqualified = q.value("runs.disqualified").toBool();
	m_disqualifiedByOrganizer = q.value("runs.disqualifiedByOrganizer").toBool();
	m_notCompeting = q.value("runs.notCompeting").toBool();
	m_missingPunch = q.value("runs.misPunch").toBool();
	m_didNotStart = q.value("runs.notStart").toBool();
	m_didNotFinish = q.value("runs.notFinish").toBool();
	m_overTime = q.value("runs.overTime").toBool();
}

void ResultStatus::fillFromTreeTableRow(const qf::core::utils::TreeTableRow &ttr)
{
	m_disqualified = ttr.value(QStringLiteral("disqualified")).toBool();
	m_disqualifiedByOrganizer = ttr.value("disqualifiedByOrganizer").toBool();
	m_notCompeting = ttr.value("notCompeting").toBool();
	m_missingPunch = ttr.value("misPunch").toBool();
	m_didNotStart = ttr.value("notStart").toBool();
	m_didNotFinish = ttr.value("notFinish").toBool();
	m_overTime = ttr.value("overTime").toBool();
}

bool ResultStatus::isOk() const
{
	return !(m_disqualified || m_notCompeting);
}

int ResultStatus::getOGTime(int time) const
{
	if (m_disqualifiedByOrganizer)
		return quickevent::core::og::TimeMs::DISQ_TIME_MSEC;
	else if (m_notCompeting)
		return quickevent::core::og::TimeMs::NOT_COMPETITING_TIME_MSEC;
	else if (m_didNotStart)
		return quickevent::core::og::TimeMs::NOT_START_TIME_MSEC;
	else if (m_didNotFinish)
		return quickevent::core::og::TimeMs::NOT_FINISH_TIME_MSEC;
	else if (m_overTime)
		return quickevent::core::og::TimeMs::OVERTIME_TIME_MSEC;
	else if (m_missingPunch && m_disqualified)
		return quickevent::core::og::TimeMs::MISPUNCH_TIME_MSEC;
	else
		return time;
}

QString ResultStatus::toXmlExportString() const
{
	if (m_disqualified) {
		if (m_disqualifiedByOrganizer)
			return QStringLiteral("Disqualified");
		else if (m_missingPunch)
			return QStringLiteral("MissingPunch");
		else if (m_didNotStart)
			return QStringLiteral("DidNotStart");
		else if (m_didNotFinish)
			return QStringLiteral("DidNotFinish");
		else if (m_overTime)
			return QStringLiteral("OverTime");
		else
			return QStringLiteral("???");
	}
	else if (m_notCompeting)
		return QStringLiteral("NotCompeting");
	else
		return QStringLiteral("OK");
}


QString ResultStatus::toEmmaExportString() const
{
	if (m_disqualified)	{
		if (m_disqualifiedByOrganizer)
			return QStringLiteral("DISQ");
		else if (m_missingPunch)
			return QStringLiteral("MP  ");
		else if (m_didNotStart)
			return QStringLiteral("DNS ");
		else if (m_didNotFinish)
			return QStringLiteral("DNF ");
		else if (m_overTime)
			return QStringLiteral("OVRT");
		else
			return QStringLiteral("???");
	}
	else if (m_notCompeting)
		return QStringLiteral("NC  ");
	else
		return QStringLiteral("O.K.");
}

QString ResultStatus::toHtmlExportString() const
{
	if (m_disqualified)	{
		if (m_disqualifiedByOrganizer)
			return QStringLiteral("DISQ");
		else if (m_missingPunch)
			return QStringLiteral("MP");
		else if (m_didNotStart)
			return QStringLiteral("DNS");
		else if (m_didNotFinish)
			return QStringLiteral("DNF");
		else if (m_overTime)
			return QStringLiteral("OVRT");
		else
			return QStringLiteral("???");
	}
	else if (m_notCompeting)
		return QStringLiteral("NC");
	else
		return QStringLiteral("OK");
}

QString ResultStatus::toString() const
{
	if (m_disqualified)	{
		if (m_disqualifiedByOrganizer)
			return QCoreApplication::translate("ResultStatus","DISQ", "Disqualified");
		else if (m_missingPunch)
			return QCoreApplication::translate("ResultStatus","MP", "Missing Punch");
		else if (m_didNotStart)
			return QCoreApplication::translate("ResultStatus","DNS", "Did Not Start");
		else if (m_didNotFinish)
			return QCoreApplication::translate("ResultStatus","DNF", "Did Not Finish");
		else if (m_overTime)
			return QCoreApplication::translate("ResultStatus","OVRT", "Over Time");
		else
			return QStringLiteral("???");
	}
	else if (m_notCompeting)
		return QCoreApplication::translate("ResultStatus","NC", "Not Competing");
	else
		return QCoreApplication::translate("ResultStatus","OK");
}

} // namespace core
} // namespace quickevent
