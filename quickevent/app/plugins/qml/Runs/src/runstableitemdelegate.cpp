#include "runstableitemdelegate.h"
#include "runswidget.h"

#include <Event/eventplugin.h>

#include <quickevent/og/timems.h>

#include <qf/qmlwidgets/tableview.h>

#include <qf/core/model/sqltablemodel.h>

#include <qf/core/sql/connection.h>
#include <qf/core/sql/query.h>
#include <qf/core/log.h>

#include <QPainter>

namespace qfs = qf::core::sql;

RunsTableItemDelegate::RunsTableItemDelegate(qf::qmlwidgets::TableView * parent)
	: Super(parent)
{
}

void RunsTableItemDelegate::setHighlightedClassId(int class_id)
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

void RunsTableItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &ix) const
{
	qf::qmlwidgets::TableView *v = view();
	if(v) do {
		auto *m = v->model();
		auto *tm = qobject_cast<qf::core::model::SqlTableModel*>(v->tableModel());
		if(m && tm) {
			if(m_highlightedClassId > 0 && m_classInterval > 0 && isStartTimeHighlightVisible()) {
				auto cd = tm->columnDefinition(ix.column());
				if(cd.matchesSqlId(QStringLiteral("runs.startTimeMs"))) {
					quickevent::og::TimeMs tms = m->data(ix, qf::core::model::TableModel::RawValueRole).value<quickevent::og::TimeMs>();
					int start_ms = tms.msec();
					int prev_start_ms = m_classStart;
					//int table_row = v->toTableModelRowNo(index.row());
					int reg_col = v->logicalColumnIndex("registration");
					QF_ASSERT(reg_col >= 0, "Bad registration column!", break);
					QString club = m->data(ix.sibling(ix.row(), reg_col), qf::core::model::TableModel::RawValueRole).toString().mid(0, 3).trimmed();
					QString prev_club;
					if(ix.row() > 0) {
						tms = m->data(ix.sibling(ix.row() - 1, ix.column()), qf::core::model::TableModel::RawValueRole).value<quickevent::og::TimeMs>();
						prev_start_ms = tms.msec();
						prev_club = m->data(ix.sibling(ix.row() - 1, reg_col), qf::core::model::TableModel::RawValueRole).toString().mid(0, 3).trimmed();
					}
					//qfWarning() << ix.row() << club << prev_club;
					QColor c;
					if((start_ms > m_classStart && start_ms == prev_start_ms) || ((start_ms - prev_start_ms) % m_classInterval) != 0) {
						c = Qt::red;
					}
					else if((start_ms - prev_start_ms) > m_classInterval) {
						//qfInfo() << (start_ms - prev_start_ms) << m_classInterval;
						c = QColor("lime");
					}
					else if(club == prev_club) {
						//qfInfo() << (start_ms - prev_start_ms) << m_classInterval;
						c = QColor(Qt::magenta);
					}
					if(c.isValid()) {
						painter->fillRect(option.rect, c);
					}
				}
			}
		}
	} while(false);
	Super::paint(painter, option, ix);
}
