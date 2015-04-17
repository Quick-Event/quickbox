#include "ganttitem.h"
#include "startslotitem.h"
#include "classitem.h"

#include <Event/eventplugin.h>

#include <qf/qmlwidgets/framework/mainwindow.h>

#include <qf/core/sql/query.h>
#include <qf/core/sql/connection.h>
#include <qf/core/sql/querybuilder.h>

namespace qfs = qf::core::sql;

using namespace drawing;

GanttItem::GanttItem(QGraphicsItem *parent)
	: Super(parent)
{
}

StartSlotItem *GanttItem::startSlotItem(int ix)
{
	while(ix >= startSlotItemCount()) {
		m_startSlotItems << new StartSlotItem(this);
	}
	return m_startSlotItems.value(ix);
}

Event::EventPlugin *GanttItem::eventPlugin()
{
	qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
	auto plugin = qobject_cast<Event::EventPlugin *>(fwk->plugin("Event"));
	QF_ASSERT(plugin != nullptr, "Bad plugin", return nullptr);
	return plugin;
}

void GanttItem::load()
{
	qfLogFuncFrame();
	int stage_id = eventPlugin()->currentStageId();
	Event::Stage stage = eventPlugin()->stage(stage_id);
	DrawingConfig dc(stage.drawingConfig());
	QVariantList stsllst = dc.startSlots();
	for(auto v : stsllst) {
		StartSlotData sd(v.toMap());
		StartSlotItem *ssi = addStartSlotItem();
		ssi->setData(sd);
	}
	qfs::Query q(qfs::Connection::forName());
	qf::core::sql::QueryBuilder qb1;
	qb1.select("COUNT(*)")
			.from("runs")
			.join("runs.competitorId", "competitors.id")
			.where("competitors.classId=classdefs.classId")
			.where("NOT runs.offRace")
			.where("runs.stageId=" QF_IARG(stage_id));
	qf::core::sql::QueryBuilder qb;
	qb.select2("classdefs", "*")
			.select2("classes", "name AS className")
			.select2("courses", "name AS courseName")
			.select2("codes", "code AS firstCode")
			.select("(" + qb1.toString() + ") AS runsCount")
			.from("classdefs")
			.join("classdefs.classId", "classes.id")
			.join("classdefs.courseId", "courses.id")
			.joinRestricted("classdefs.courseId", "coursecodes.courseId", "coursecodes.position=1")
			.join("coursecodes.codeId", "codes.id")
			.where("startSlotIndex < 0")
			.where("classdefs.stageId=" QF_IARG(stage_id))
			.orderBy("startSlotIndex DESC, startSlotPosition, firstCode, classdefs.courseId");
	int curr_course_id = -1;
	StartSlotItem *slot_item = nullptr;
	QString qs = qb.toString();
	qfDebug() << qs;
	q.exec(qs);
	while(q.next()) {
		ClassData cd(q);
		int ix = cd.startSlotIndex();
		if(ix >= 0) {
			slot_item = startSlotItem(ix);
		}
		else {
			if(cd.courseId() != curr_course_id) {
				curr_course_id = cd.courseId();
				slot_item = addStartSlotItem();
			}
		}
		auto *class_it = slot_item->addClassItem();
		class_it->setData(cd);
	}
	updateGeometry();
}

void GanttItem::updateGeometry()
{
	int pos_y = 0;
	for (int i = 0; i < startSlotItemCount(); ++i) {
		StartSlotItem *it = startSlotItem(i);
		it->setSlotNumber(i + 1);
		it->updateGeometry();
		it->setPos(QPoint(0, pos_y));
		pos_y += it->rect().height();
	}
}

