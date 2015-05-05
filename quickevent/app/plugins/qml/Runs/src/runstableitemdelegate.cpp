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

void RunsTableItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	qf::qmlwidgets::TableView *v = view();
	if(v) {
		auto *m = v->model();
		auto *tm = qobject_cast<qf::core::model::SqlTableModel*>(v->tableModel());
		if(m && tm) {
			if(m_highlightedClassId > 0 && m_classInterval > 0 && isStartTimeHighlightVisible()) {
				auto cd = tm->columnDefinition(index.column());
				if(cd.matchesSqlId(QStringLiteral("runs.startTimeMs"))) {
					quickevent::og::TimeMs tms = m->data(index, qf::core::model::TableModel::RawValueRole).value<quickevent::og::TimeMs>();
					int start_ms = tms.msec();
					int prev_start_ms = m_classStart;
					if(index.row() > 0) {
						tms = m->data(index.sibling(index.row() - 1, index.column()), qf::core::model::TableModel::RawValueRole).value<quickevent::og::TimeMs>();
						prev_start_ms = tms.msec();
					}
					//qfWarning() << index.row() << start_ms << prev_start_ms << "diff:" << (start_ms - prev_start_ms) << m_classInterval;
					QColor c;
					if((start_ms - prev_start_ms) % m_classInterval) {
						c = Qt::red;
					}
					else if((start_ms - prev_start_ms) > m_classInterval) {
						//qfInfo() << (start_ms - prev_start_ms) << m_classInterval;
						c = QColor("lime");
					}
					if(c.isValid()) {
						painter->fillRect(option.rect, c);
					}
				}
			}
		}
	}
	Super::paint(painter, option, index);
}
