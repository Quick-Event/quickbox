#include "runstatus.h"
#include "og/timems.h"
#include <QCoreApplication>

namespace quickevent {
namespace core {

RunStatus::RunStatus(qf::core::sql::Query &q)
{
	fillFromQuery(q);
}

RunStatus::RunStatus(const qf::core::utils::TreeTableRow &ttr)
{
	fillFromTreeTableRow(ttr);
}

QString RunStatus::dbRunsColumnList()
{
	return QStringLiteral(" disqualified, disqualifiedByOrganizer, notCompeting, misPunch, notStart, notFinish, overTime");
}

void RunStatus::fillFromQuery(qf::core::sql::Query &q)
{
	m_disqualified = q.value("runs.disqualified").toBool();
	m_disqualifiedByOrganizer = q.value("runs.disqualifiedByOrganizer").toBool();
	m_notCompeting = q.value("runs.notCompeting").toBool();
	m_missingPunch = q.value("runs.misPunch").toBool();
	m_didNotStart = q.value("runs.notStart").toBool();
	m_didNotFinish = q.value("runs.notFinish").toBool();
	m_overTime = q.value("runs.overTime").toBool();
}

void RunStatus::fillFromTreeTableRow(const qf::core::utils::TreeTableRow &ttr)
{
	m_disqualified = ttr.value(QStringLiteral("disqualified")).toBool();
	m_disqualifiedByOrganizer = ttr.value("disqualifiedByOrganizer").toBool();
	m_notCompeting = ttr.value("notCompeting").toBool();
	m_missingPunch = ttr.value("misPunch").toBool();
	m_didNotStart = ttr.value("notStart").toBool();
	m_didNotFinish = ttr.value("notFinish").toBool();
	m_overTime = ttr.value("overTime").toBool();
}

bool RunStatus::isOk() const
{
	return !(m_disqualified || m_notCompeting);
}

int RunStatus::getOGTime(int time) const
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

QString RunStatus::toXmlExportString() const
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


QString RunStatus::toEmmaExportString() const
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

QString RunStatus::toHtmlExportString() const
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

QString RunStatus::toString() const
{
	if (m_disqualified)	{
		if (m_disqualifiedByOrganizer)
			return QCoreApplication::translate("RunStatus","DISQ", "Disqualified");
		else if (m_missingPunch)
			return QCoreApplication::translate("RunStatus","MP", "Missing Punch");
		else if (m_didNotStart)
			return QCoreApplication::translate("RunStatus","DNS", "Did Not Start");
		else if (m_didNotFinish)
			return QCoreApplication::translate("RunStatus","DNF", "Did Not Finish");
		else if (m_overTime)
			return QCoreApplication::translate("RunStatus","OVRT", "Over Time");
		else
			return QStringLiteral("???");
	}
	else if (m_notCompeting)
		return QCoreApplication::translate("RunStatus","NC", "Not Competing");
	else
		return QCoreApplication::translate("RunStatus","OK");
}

} // namespace core
} // namespace quickevent
