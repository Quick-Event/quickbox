#include "runstablemodel.h"
#include "runswidget.h"

#include <Event/eventplugin.h>

#include <qf/core/sql/connection.h>
#include <qf/core/sql/query.h>

#include <quickevent/og/timems.h>

namespace qfs = qf::core::sql;

RunsTableModel::RunsTableModel(QObject *parent)
	: Super(parent)
{

}

void RunsTableModel::setHighlightedClassId(int class_id)
{
	if(m_highlightedClassId == class_id)
		return;
	m_highlightedClassId = 0;
	int stage_id = RunsWidget::eventPlugin()->currentStageId();
	qf::core::sql::QueryBuilder qb;
	qb.select2("classdefs", "startTimeMin, startIntervalMin, vacantsBefore, vacantEvery, vacantsAfter")
			.from("classdefs")
			.where("stageId=" QF_IARG(stage_id))
			.where("classId=" QF_IARG(class_id));
	qfs::Query q(qfs::Connection::forName());
	q.exec(qb.toString(), qf::core::Exception::Throw);
	if(q.next()) {
		m_highlightedClassId = class_id;
		m_classInterval = q.value("startIntervalMin").toInt() * 60 * 1000;
		m_classStart = q.value("startTimeMin").toInt() * 60 * 1000;
	}
}

QVariant RunsTableModel::data(const QModelIndex &index, int role) const
{
	QVariant ret = Super::data(index, role);
	if(m_highlightedClassId > 0 && m_classInterval > 0 && startTimeHighlightRowsOrder().count()) {
		if(role == Qt::BackgroundRole) {
			auto cd = columnDefinition(index.column());
			if(cd.matchesSqlId(QStringLiteral("runs.startTimeMs"))) {
				quickevent::og::TimeMs tms = value(startTimeHighlightRowsOrder().value(index.row()), index.column()).value<quickevent::og::TimeMs>();
				int start_ms = tms.msec();
				int prev_start_ms = m_classStart;
				if(startTimeHighlightRowsOrder().value(index.row()) > 0) {
					tms = value(startTimeHighlightRowsOrder().value(index.row() - 1), index.column()).value<quickevent::og::TimeMs>();
					prev_start_ms = tms.msec();
				}
				qfWarning() << index.row() << start_ms << prev_start_ms << "diff:" << (start_ms - prev_start_ms) << m_classInterval;
				if((start_ms - prev_start_ms) % m_classInterval) {
					ret = QColor(Qt::red);
				}
				else if((start_ms - prev_start_ms) > m_classInterval) {
					qfInfo() << (start_ms - prev_start_ms) << m_classInterval;
					ret = QColor("lime");
				}
			}
		}
	}
	return ret;
}

