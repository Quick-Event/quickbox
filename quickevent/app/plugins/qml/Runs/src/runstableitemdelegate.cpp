#include "runstableitemdelegate.h"
#include "runswidget.h"

#include <Event/eventplugin.h>

#include <quickevent/core/og/timems.h>

#include <qf/qmlwidgets/tableview.h>

#include <qf/core/model/sqltablemodel.h>
#include <qf/core/sql/connection.h>
#include <qf/core/sql/query.h>
#include <qf/core/log.h>
#include <qf/core/assert.h>

#include <QPainter>

namespace qfs = qf::core::sql;

RunsTableItemDelegate::RunsTableItemDelegate(qf::qmlwidgets::TableView * parent)
	: Super(parent), m_classStart(), m_classInterval()
{
}

void RunsTableItemDelegate::setHighlightedClassId(int class_id, int stage_id)
{
	if(m_highlightedClassId == class_id && m_stageId == stage_id)
		return;
	m_stageId = stage_id;
	m_highlightedClassId = 0;
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

void RunsTableItemDelegate::paintBackground(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	qf::qmlwidgets::TableView *v = view();
	if(v) do {
		auto *m = v->model();
		auto *tm = qobject_cast<qf::core::model::SqlTableModel*>(v->tableModel());
		if(m && tm) {
			//qfInfo() << m_highlightedClassId << m_classInterval << isStartTimeHighlightVisible();
			if(m_highlightedClassId > 0 && m_classInterval > 0 && isStartTimeHighlightVisible()) {
				auto cd = tm->columnDefinition(index.column());
				if(cd.matchesSqlId(QStringLiteral("runs.startTimeMs"))) {
					QVariant stime_v = m->data(index, Qt::EditRole);
					quickevent::core::og::TimeMs stime = stime_v.value<quickevent::core::og::TimeMs>();
					if(!stime.isValid())
						break;

					int start_ms = stime.msec();
					int prev_start_ms = m_classStart;
					//int table_row = v->toTableModelRowNo(index.row());
					int reg_col = v->logicalColumnIndex("registration");
					QF_ASSERT(reg_col >= 0, "Bad registration column!", break);
					QString club = m->data(index.sibling(index.row(), reg_col), Qt::EditRole).toString().mid(0, 3).trimmed();
					QString prev_club;
					if(index.row() > 0) {
						stime = m->data(index.sibling(index.row() - 1, index.column()), Qt::EditRole).value<quickevent::core::og::TimeMs>();
						prev_start_ms = stime.msec();
						prev_club = m->data(index.sibling(index.row() - 1, reg_col), Qt::EditRole).toString().mid(0, 3).trimmed();
					}
					//qfWarning() << ix.row() << club << prev_club;
					QColor c;
					//qfInfo() << prev_start_ms << start_ms << (start_ms - prev_start_ms) << m_classInterval;
					if((start_ms > m_classStart && start_ms == prev_start_ms) || ((start_ms - prev_start_ms) % m_classInterval) != 0) {
						c = Qt::red;
						c.setAlphaF(0.5);
					}
					else if(((start_ms - prev_start_ms) > m_classInterval)
							|| ((index.row() == 0) && (start_ms > m_classStart))) {
						c = QColor("lime");
					}
					else if(club == prev_club) {
						//qfInfo() << (start_ms - prev_start_ms) << m_classInterval;
						c = QColor(Qt::magenta);
					}
					if(c.isValid()) {
						painter->fillRect(option.rect, c);
						return;
					}
				}
			}
		}
	} while(false);
	Super::paintBackground(painter, option, index);
}
