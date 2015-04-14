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
	setDisplayUnit(QFontMetrics(QFont()).lineSpacing());
}

StartSlotItem *GanttItem::slotItem(int ix)
{
	while(ix >= slotItemCount()) {
		m_slotItems << new StartSlotItem(this);
	}
	return m_slotItems.value(ix);
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
	int stage_id = eventPlugin()->currentStageId();
	Event::Stage stage = eventPlugin()->stage(stage_id);
	DrawingConfig dc(stage.drawingConfig());
	QVariantList stsllst = dc.startSlots();
	for(auto v : stsllst) {
		StartSlotData sd(v.toMap());
		StartSlotItem *ssi = addSlotItem();
		ssi->setData(sd);
	}
	qfs::Query q(qfs::Connection::forName());
	q.exec("SELECT * FROM classdefs WHERE startSlotIndex >= 0 AND stageId=" QF_IARG(stage_id) " ORDER BY startSlotIndex, startSlotPosition");
	while(q.next()) {
		ClassData cd(q);
		int slot_ix = cd.startSlotIndex();
		auto *slot_item = slotItem(slot_ix);
		auto *class_it = slot_item->addClassItem();
		class_it->setData(cd);
	}
	qf::core::sql::QueryBuilder qb1;
	qb1.select("codes.code").from("coursecodes").joinRestricted("coursecodes.codeId", "codes.id", "coursecodes.courseId=classdefs.courseId").orderBy("coursecodes.position").limit(1);
	qf::core::sql::QueryBuilder qb;
	qb.select2("classdefs", "*")
			.select("(" + qb1.toString() + ") AS firstCode")
			.from("classdefs")
			.join("classdefs.courseId")
			.where("startSlotIndex")
			.where("stageId=" QF_IARG(stage_id))
			.orderBy("courseId");
	int curr_course_id = -1;
	StartSlotItem *slot_item = nullptr;
	while(q.next()) {
		ClassData cd(q);
		if(cd.courseId() != curr_course_id) {
			slot_item = addSlotItem();
		}
		auto *class_it = slot_item->addClassItem();
		class_it->setData(cd);
	}
}

