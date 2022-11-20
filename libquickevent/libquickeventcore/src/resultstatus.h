#pragma once

#include "quickeventcoreglobal.h"
#include <qf/core/sql/query.h>
#include <qf/core/utils/treetable.h>

namespace quickevent {
namespace core {

class QUICKEVENTCORE_DECL_EXPORT ResultStatus
{
public:
	ResultStatus() = default;
	ResultStatus(qf::core::sql::Query &q);
	ResultStatus(const qf::core::utils::TreeTableRow &ttr);
	// set all variables from query (table runs)
	void fillFromQuery(qf::core::sql::Query &q);
	void fillFromTreeTableRow(const qf::core::utils::TreeTableRow &ttr);

	bool isOk() const;
	int getOGTime(int time) const;

	QString statusXml() const;
	QString statusEmmaTxt() const;
	QString statusResultExport() const;
	QString statusText() const;

	bool disqualified() const { return m_disqualified; }
	bool disqualifiedByOrganizer() const { return m_disqualifiedByOrganizer; }
	bool notCompeting() const { return m_notCompeting; }
	bool missingPunch() const { return m_missingPunch; }
	bool didNotStart() const { return m_didNotStart; }
	bool didNotFinish() const { return m_didNotFinish; }
	bool overTime() const { return m_overTime; }

	static QString dbRunsColumnList();
private:
	bool m_disqualified = false;
	bool m_disqualifiedByOrganizer = false;
	bool m_notCompeting = false;
	bool m_missingPunch = false;
	bool m_didNotStart = false;
	bool m_didNotFinish = false;
	bool m_overTime = false;
};

}}
