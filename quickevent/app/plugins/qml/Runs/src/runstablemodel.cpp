#include "runstablemodel.h"
#include "runswidget.h"

#include <Event/eventplugin.h>

#include <qf/core/sql/connection.h>
#include <qf/core/sql/query.h>

namespace qfs = qf::core::sql;

RunsTableModel::RunsTableModel(QObject *parent)
	: Super(parent)
{

}

void RunsTableModel::highlightDrawForClass(int class_id)
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
	if(role == Qt::DecorationRole) {
		auto cd = columnDefinition(index.column());
		if(cd.matchesSqlId(QStringLiteral("runs.startTimeMs"))) {
			int start_time = ret.toInt();
		}
	}
	return ret;
}

