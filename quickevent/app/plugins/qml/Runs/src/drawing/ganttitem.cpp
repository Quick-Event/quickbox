#include "ganttitem.h"
#include "startslotitem.h"
#include "classitem.h"
#include "ganttruler.h"
#include "ganttscene.h"

#include <Event/eventplugin.h>

#include <qf/qmlwidgets/framework/mainwindow.h>

#include <qf/core/sql/query.h>
#include <qf/core/sql/connection.h>
#include <qf/core/sql/querybuilder.h>

namespace qfs = qf::core::sql;

using namespace drawing;

GanttItem::GanttItem(QGraphicsItem *parent)
	: Super(parent), IGanttItem(this)
{
	m_ganttRuler = new GanttRuler(this);
}

int GanttItem::startSlotItemIndex(const StartSlotItem *it) const
{
	return m_startSlotItems.indexOf((StartSlotItem *)it);
}

StartSlotItem *GanttItem::startSlotItemAt(int ix, bool throw_ex)
{
	StartSlotItem *ret = m_startSlotItems.value(ix);
	if(!ret && throw_ex)
		QF_EXCEPTION(QString("Invalid item index %1, item count %2!").arg(ix).arg(startSlotItemCount()));
	return ret;
}

void GanttItem::insertStartSlotItem(int ix, StartSlotItem *it)
{
	QF_ASSERT(it != nullptr, "Item == NULL", return);
	m_startSlotItems.insert(ix, it);
}

StartSlotItem *GanttItem::addStartSlotItem()
{
	auto *it = new StartSlotItem(this);
	insertStartSlotItem(startSlotItemCount(), it);
	return it;
}

Event::EventPlugin *GanttItem::eventPlugin()
{
	qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
	auto plugin = qobject_cast<Event::EventPlugin *>(fwk->plugin("Event"));
	QF_ASSERT(plugin != nullptr, "Bad plugin", return nullptr);
	return plugin;
}
/*
ClassItem *GanttItem::takeClassItem(int slot_ix, int class_ix)
{
	auto *si = startSlotItemAt(slot_ix, !qf::core::Exception::Throw);
	if(si) {
		return si->takeClassItemAt(class_ix);
	}
	return nullptr;
}

void GanttItem::insertClassItem(int slot_ix, int class_ix, ClassItem *it)
{
	auto *si = startSlotItemAt(slot_ix, !qf::core::Exception::Throw);
	if(si) {
		return si->insertClassItem(class_ix, it);
	}
}
*/
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
			.where("classdefs.stageId=" QF_IARG(stage_id))
			.orderBy("startSlotIndex, startSlotPosition, firstCode, classdefs.courseId");
	int curr_course_id = -1;
	int curr_slot_ix = -1;
	StartSlotItem *slot_item = nullptr;
	QString qs = qb.toString();
	qfDebug() << qs;
	q.exec(qs);
	while(q.next()) {
		ClassData cd(q);
		int slot_ix = cd.startSlotIndex();
		if(slot_ix >= 0) {
			if(slot_ix > curr_slot_ix) {
				slot_item = addStartSlotItem();
				curr_slot_ix = slot_ix;
			}
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

void GanttItem::save()
{
	qfLogFuncFrame();
	int stage_id = eventPlugin()->currentStageId();
	QString qs = "UPDATE classdefs SET startSlotIndex=:startSlotIndex, startSlotPosition=:startSlotPosition WHERE classdefs.id=:id AND stageId=:stageId";
	qfs::Query q(qfs::Connection::forName());
	q.prepare(qs, qf::core::Exception::Throw);
	for (int i = 0; i < startSlotItemCount(); ++i) {
		StartSlotItem *slot_it = startSlotItemAt(i);
		for (int j = 0; j < slot_it->classItemCount(); ++j) {
			ClassItem *class_it = slot_it->classItemAt(j);
			auto dt = class_it->data();
			qfDebug() << dt.id() << dt.className() << "slot:" << dt.startSlotIndex() << "pos:" << dt.startSlotPosition();
			q.bindValue(":startSlotIndex", dt.startSlotIndex());
			q.bindValue(":startSlotPosition", dt.startSlotPosition());
			q.bindValue(":id", dt.id());
			q.bindValue(":stageId", stage_id);
			q.exec(qf::core::Exception::Throw);
		}
	}
}

void GanttItem::updateGeometry()
{
	qfLogFuncFrame();
	int pos_y = 0;
	double w = 0;
	for (int i = 0; i < startSlotItemCount(); ++i) {
		StartSlotItem *it = startSlotItemAt(i);
		it->setSlotNumber(i + 1);
		it->updateGeometry();
		it->setPos(QPoint(0, pos_y));
		qfDebug() << i << it->rect().height() << it;
		pos_y += it->rect().height();
		w = qMax(w, it->rect().right());
	}
	for (int i = 0; i < startSlotItemCount(); ++i) {
		StartSlotItem *it = startSlotItemAt(i);
		it->setClassAreaWidth(w);
	}
	QRectF r;
	r.setWidth(w);
	r.setHeight(pos_y);
	{
		QRectF rr = r;
		rr.setHeight(3 * ganttScene()->displayUnit());
		rr.moveTop(-rr.height());
		m_ganttRuler->setRect(rr);
		r |= rr;
	}
	setRect(r);
}

void GanttItem::moveClassItem(int from_slot_ix, int from_class_ix, int to_slot_ix, int to_class_ix)
{
	auto *slot1 = startSlotItemAt(from_slot_ix);
	auto *class_it = slot1->takeClassItemAt(from_class_ix);
	auto *slot2 = startSlotItemAt(to_slot_ix);
	slot2->insertClassItem(to_class_ix, class_it);
	updateGeometry();
}





