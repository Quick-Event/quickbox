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

#include <QJsonDocument>

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

StartSlotItem *GanttItem::takeStartSlotItemAt(int ix)
{
	StartSlotItem *ret = m_startSlotItems.takeAt(ix);
	ret->setParentItem(nullptr);
	return ret;
}

void GanttItem::insertStartSlotItem(int ix, StartSlotItem *it)
{
	QF_ASSERT(it != nullptr, "Item == NULL", return);
	m_startSlotItems.insert(ix, it);
	it->setParentItem(this);
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
	Event::StageData stage = eventPlugin()->stageData(stage_id);
	DrawingConfig dc(stage.drawingConfig());
	QVariantList stsllst = dc.startSlots();

	qfs::Query q(qfs::Connection::forName());
	qf::core::sql::QueryBuilder qb1;
	qb1.select("competitors.classId")
			.select("COUNT(*) AS runsCount")
			.select("MIN(runs.startTimeMs) AS minStartTime")
			.select("MAX(runs.startTimeMs) AS maxStartTime")
			.from("competitors")
			.joinRestricted("competitors.id", "runs.competitorId", "NOT runs.offRace AND runs.stageId=" QF_IARG(stage_id))
			.groupBy("competitors.classId")
			.as("classruns");
	qf::core::sql::QueryBuilder qb;
	qb.select2("classdefs", "*")
			.select2("classes", "name AS className")
			.select2("courses", "name AS courseName")
			.select2("codes", "code AS firstCode")
			.select2("classruns", "runsCount, minStartTime, maxStartTime")
			.from("classdefs")
			.join("classdefs.classId", "classes.id")
			.join("classdefs.courseId", "courses.id")
			.joinRestricted("classdefs.courseId", "coursecodes.courseId", "coursecodes.position=1")
			.join("coursecodes.codeId", "codes.id")
			.joinQuery("classdefs.classId", qb1, "classId")
			.where("classdefs.stageId=" QF_IARG(stage_id))
			.orderBy("startSlotIndex, startTimeMin, firstCode, classdefs.courseId");
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
				StartSlotData sd(stsllst.value(slot_ix).toMap());
				slot_item->setData(sd);
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
	checkClassClash();
}

void GanttItem::save()
{
	qfLogFuncFrame();
	int stage_id = eventPlugin()->currentStageId();
	{
		Event::StageData stage = eventPlugin()->stageData(stage_id);
		DrawingConfig dc(stage.drawingConfig());
		QVariantList start_slots;
		for (int i = 0; i < startSlotItemCount(); ++i) {
			StartSlotItem *slot_it = startSlotItemAt(i);
			StartSlotData sd = slot_it->data();
			start_slots << sd;
		}
		dc.setStartSlots(start_slots);
		QJsonDocument jsd = QJsonDocument::fromVariant(dc);
		QString dc_str = QString::fromUtf8(jsd.toJson(QJsonDocument::Compact));

		QString qs = "UPDATE stages SET drawingConfig=:drawingConfig WHERE id=:id";
		qfs::Query q(qfs::Connection::forName());
		q.prepare(qs, qf::core::Exception::Throw);
		q.bindValue(":drawingConfig", dc_str);
		q.bindValue(":id", stage_id);
		q.exec(qf::core::Exception::Throw);
		eventPlugin()->clearStageDataCache();
	}
	{
		QString qs = "UPDATE classdefs SET"
					 "  startSlotIndex=:startSlotIndex,"
					 "  startTimeMin=:startTimeMin"
					 " WHERE classdefs.id=:id AND stageId=:stageId";
		qfs::Query q(qfs::Connection::forName());
		q.prepare(qs, qf::core::Exception::Throw);
		for (int i = 0; i < startSlotItemCount(); ++i) {
			StartSlotItem *slot_it = startSlotItemAt(i);
			for (int j = 0; j < slot_it->classItemCount(); ++j) {
				ClassItem *class_it = slot_it->classItemAt(j);
				auto dt = class_it->data();
				qfDebug() << dt.id() << dt.className() << "slot:" << dt.startSlotIndex() << "start time:" << dt.startTimeMin();
				q.bindValue(":startSlotIndex", dt.startSlotIndex());
				q.bindValue(":startTimeMin", dt.startTimeMin());
				//q.bindValue(":isDrawnIn", dt.isDrawnIn());
				q.bindValue(":id", dt.id());
				q.bindValue(":stageId", stage_id);
				q.exec(qf::core::Exception::Throw);
			}
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

void GanttItem::checkClassClash()
{
	for (int i = 0; i < startSlotItemCount(); ++i) {
		StartSlotItem *slot_it = startSlotItemAt(i);
		for (int j = 0; j < slot_it->classItemCount(); ++j) {
			ClassItem *class_it = slot_it->classItemAt(j);
			class_it->setClashingClasses(QList<ClassItem*>());
		}
	}
	for (int i = 0; i < startSlotItemCount(); ++i) {
		StartSlotItem *slot_it = startSlotItemAt(i);
		if(slot_it->data().isIgnoreClassClashCheck())
			continue;
		for (int j = 0; j < slot_it->classItemCount(); ++j) {
			ClassItem *class_it = slot_it->classItemAt(j);
			QList<ClassItem*> clash_list = class_it->findClashes();
			if(clash_list.count()) {
				class_it->setClashingClasses(clash_list);
				for(auto *cl : clash_list) {
					cl->setClashingClasses(QList<ClassItem*>() << class_it);
				}
				return;
			}
		}
	}
}

void GanttItem::moveClassItem(int from_slot_ix, int from_class_ix, int to_slot_ix, int to_class_ix)
{
	if(from_slot_ix == to_slot_ix && (from_class_ix == to_class_ix || to_class_ix == from_class_ix + 1))
		return;
	if(from_slot_ix == to_slot_ix && from_class_ix < to_class_ix)
		to_class_ix--;
	auto *slot1 = startSlotItemAt(from_slot_ix);
	auto *class_it = slot1->takeClassItemAt(from_class_ix);
	auto *slot2 = startSlotItemAt(to_slot_ix);
	slot2->insertClassItem(to_class_ix, class_it);
	updateGeometry();
	checkClassClash();
}

void GanttItem::moveStartSlotItem(int from_slot_ix, int to_slot_ix)
{
	if(from_slot_ix == to_slot_ix)
		return;
	auto *slot = takeStartSlotItemAt(from_slot_ix);
	if(slot) {
		if(from_slot_ix < to_slot_ix)
			to_slot_ix--;
		insertStartSlotItem(to_slot_ix, slot);
		updateGeometry();
		checkClassClash();
	}
}





