#include "relaysplugin.h"
#include "../thispartwidget.h"
#include "relaydocument.h"
#include "../relaywidget.h"

#include <Event/eventplugin.h>

#include <quickevent/core/og/timems.h>
#include <quickevent/core/si/checkedcard.h>

#include <qf/qmlwidgets/framework/mainwindow.h>
#include <qf/qmlwidgets/framework/dockwidget.h>
#include <qf/qmlwidgets/dialogs/dialog.h>
#include <qf/qmlwidgets/action.h>
#include <qf/qmlwidgets/menubar.h>

#include <qf/core/sql/connection.h>
#include <qf/core/utils/treetable.h>
#include <qf/core/model/sqltablemodel.h>
#include <qf/core/log.h>
#include <qf/core/assert.h>

#include <QQmlEngine>

namespace qfw = qf::qmlwidgets;
namespace qff = qf::qmlwidgets::framework;
namespace qfd = qf::qmlwidgets::dialogs;
namespace qfm = qf::core::model;
namespace qfs = qf::core::sql;
namespace qog = quickevent::core::og;

namespace Relays {

static Event::EventPlugin* eventPlugin()
{
	qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
	auto *plugin = qobject_cast<Event::EventPlugin*>(fwk->plugin("Event"));
	QF_ASSERT_EX(plugin != nullptr, "Bad Event plugin!");
	return plugin;
}

RelaysPlugin::RelaysPlugin(QObject *parent)
	: Super(parent)
{
	connect(this, &RelaysPlugin::installed, this, &RelaysPlugin::onInstalled, Qt::QueuedConnection);
}

RelaysPlugin::~RelaysPlugin()
{
	//if(m_registrationsDockWidget)
	//	m_registrationsDockWidget->savePersistentSettingsRecursively();
}

QObject *RelaysPlugin::createRelayDocument(QObject *parent)
{
	 RelayDocument *ret = new  RelayDocument(parent);
	if(!parent) {
		qfWarning() << "Parent is NULL, created class will have QQmlEngine::JavaScriptOwnership.";
		qmlEngine()->setObjectOwnership(ret, QQmlEngine::JavaScriptOwnership);
	}
	return ret;
}

int RelaysPlugin::editRelay(int id, int mode)
{
	qfLogFuncFrame() << "id:" << id;
	auto *w = new  RelayWidget();
	w->setWindowTitle(tr("Edit Relay"));
	qfd::Dialog dlg(QDialogButtonBox::Save | QDialogButtonBox::Cancel, m_partWidget);
	dlg.setDefaultButton(QDialogButtonBox::Save);
	dlg.setCentralWidget(w);
	w->load(id, (qfm::DataDocument::RecordEditMode)mode);
	return dlg.exec();
}

void RelaysPlugin::onInstalled()
{
	qff::MainWindow *fwk = qff::MainWindow::frameWork();
	m_partWidget = new ThisPartWidget();
	fwk->addPartWidget(m_partWidget, manifest()->featureId());

	connect(eventPlugin(), &Event::EventPlugin::dbEventNotify, this, &RelaysPlugin::onDbEventNotify);

	emit nativeInstalled();
}

void RelaysPlugin::onDbEventNotify(const QString &domain, int connection_id, const QVariant &data)
{
	Q_UNUSED(connection_id)
	qfLogFuncFrame() << "domain:" << domain << "payload:" << data;
	if(domain == QLatin1String(Event::EventPlugin::DBEVENT_CARD_PROCESSED_AND_ASSIGNED)) {
		processRunnerFinished(quickevent::core::si::CheckedCard(data.toMap()));
	}
	emit dbEventNotify(domain, connection_id, data);
}

void RelaysPlugin::processRunnerFinished(const quickevent::core::si::CheckedCard &checked_card)
{
	qfLogFuncFrame() << checked_card;
	/// recalculate team times

}

namespace {

struct Leg
{
	QString name, reg;
	int runId = 0;
	int time = 0;
	int pos = 0;
	int stime = 0;
	int spos = 0;
	bool disq = false;
	bool notfinish = true;
};

struct Relay
{
	QString name;
	QVector<Leg> legs;
	int relayId = 0;
	int loss = 0;

	int time(int leg_cnt) const
	{
		int ret = 0;
		for (int i = 0; i < qMin(legs.count(), leg_cnt); ++i) {
			const Leg &leg = legs[i];
			if(leg.disq)
				return qog::TimeMs::DISQ_TIME_MSEC;
			if(leg.time == 0)
				return qog::TimeMs::NOT_FINISH_TIME_MSEC;
			ret += leg.time;
		}
		return ret;
	}
#if 0
	bool isDisq(int leg_cnt) const
	{
		for (int i = 0; i < qMin(legs.count(), leg_cnt); ++i) {
			const Leg &leg = legs[i];
			if(leg.disq)
				return true;
		}
		return false;
		/*
		leg_cnt = qMin(legs.count(), leg_cnt);
		if(leg_cnt == 0)
			return false;
		return legs[leg_cnt-1].stime == 0;
		*/
	}
	bool notFinish(int leg_cnt) const
	{
		for (int i = 0; i < qMin(legs.count(), leg_cnt); ++i) {
			const Leg &leg = legs[i];
			if(leg.time == 0 && !leg.disq)
				return true;
		}
		return false;
	}
	bool isOk(int leg_cnt) const
	{
		for (int i = 0; i < qMin(legs.count(), leg_cnt); ++i) {
			const Leg &leg = legs[i];
			if(leg.time == 0 || leg.disq)
				return false;
		}
		return true;
	}
#endif
};
}

qf::core::utils::TreeTable RelaysPlugin::nlegsResultsTable(int leg_count, int places, bool exclude_not_finish)
{
	qf::core::utils::TreeTable tt;
	tt.setValue("event", eventPlugin()->eventConfig()->value("event"));
	tt.setValue("stageStart", eventPlugin()->stageStartDateTime(1));
	tt.appendColumn("className", QVariant::String);
	qfs::QueryBuilder qb;
	qb.select2("classes", "id, name")
			.from("classes")
			.orderBy("classes.name");
	qfs::Query q;
	q.execThrow(qb.toString());
	while(q.next()) {
		qf::core::utils::TreeTableRow rr = tt.appendRow();
		rr.setValue("className", q.value("classes.name"));
		qf::core::utils::TreeTable tt2 = nlegsResultsTable(q.value("classes.id").toInt(), leg_count, places, exclude_not_finish);
		rr.appendTable(tt2);
	}
	return tt;
}

qf::core::utils::TreeTable RelaysPlugin::nlegsResultsTable(int class_id, int leg_count, int places, bool exclude_not_finish)
{
	int max_leg = 0;
	qfs::Query q;
	{
		qfs::QueryBuilder qb;
		qb.select("relayLegCount")
			.from("classdefs")
			.where("classId=" QF_IARG(class_id));
		q.execThrow(qb.toString());
		if(q.next())
			max_leg = q.value(0).toInt();
	}
	if(max_leg == 0) {
		qfError() << "Leg count not defined for class id:" << class_id;
		return qf::core::utils::TreeTable();
	}
	if(leg_count > max_leg)
		leg_count = max_leg;

	QList<Relay> relays;
	//QStringList relay_ids;
	{
		qfs::QueryBuilder qb;
		qb.select2("relays", "id, club, name, number")
				.select2("clubs", "name")
				.from("relays")
				.join("relays.club", "clubs.abbr")
				.where("relays.classId=" QF_IARG(class_id));
		q.execThrow(qb.toString());
		while(q.next()) {
			Relay r;
			r.relayId = q.value("relays.id").toInt();
			r.name = (q.value("relays.number").toString()
					+ ' ' + q.value("relays.club").toString()
					+ ' ' + q.value("relays.name").toString()
					+ ' ' + q.value("clubs.name").toString()).trimmed();
			for (int i = 0; i < leg_count; ++i)
				r.legs << Leg();
			relays << r;
			//relay_ids << QString::number(r.relayId);
		}
	}
	{
		qfs::QueryBuilder qb;
		qb.select2("competitors", "id, registration")
				.select2("runs", "id, relayId, leg")
				.select("COALESCE(competitors.lastName, '') || ' ' || COALESCE(competitors.firstName, '') AS competitorName")
				.from("runs")
				.join("runs.competitorId", "competitors.id")
				.joinRestricted("runs.relayId", "relays.id", "relays.classId=" QF_IARG(class_id), qfs::QueryBuilder::INNER_JOIN)
				//.where("runs.relayId IN (" + relay_ids.join(',') + ")")
				.where("runs.leg>0 AND runs.leg<=" + QString::number(leg_count))
				.orderBy("runs.relayId, runs.leg");
		q.execThrow(qb.toString());
		while(q.next()) {
			int relay_id = q.value("runs.relayId").toInt();
			for (int i = 0; i < relays.count(); ++i) {
				if(relays[i].relayId == relay_id) {
					Relay &relay = relays[i];
					int legno = q.value("runs.leg").toInt();
					Leg &leg = relay.legs[legno - 1];
					leg.name = q.value("competitorName").toString();
					leg.runId = q.value("runs.id").toInt();
					leg.reg = q.value("competitors.registration").toString();
					break;
				}
			}
		}
	}
	for (int legno = 1; legno <= leg_count; ++legno) {
		qfs::QueryBuilder qb;
		qb.select2("runs", "id, relayId, timeMs, disqualified")
				.from("runs")
				.joinRestricted("runs.relayId", "relays.id",
								"relays.classId=" QF_IARG(class_id)
								" AND runs.leg=" QF_IARG(legno)
								" AND runs.isRunning"
								" AND NOT runs.notCompeting"
								" AND runs.finishTimeMs>0"
								, qfs::QueryBuilder::INNER_JOIN)
				.orderBy("runs.disqualified, runs.timeMs");
		q.execThrow(qb.toString());
		int run_pos = 1;
		while(q.next()) {
			int relay_id = q.value("runs.relayId").toInt();
			for (int i = 0; i < relays.count(); ++i) {
				if(relays[i].relayId == relay_id) {
					int run_id = q.value("runs.id").toInt();
					Relay &relay = relays[i];
					Leg &leg = relay.legs[legno - 1];
					if(leg.runId != run_id) {
						qfError() << "internal error, leg:" << legno << "runId check:" << leg.runId << "should equal" << run_id;
					}
					else {
						leg.notfinish = false;
						leg.disq = q.value("runs.disqualified").toBool();
						leg.time = q.value("timeMs").toInt();
						leg.pos = leg.disq? 0: run_pos;
						run_pos++;
					}
					break;
				}
			}
		}
	}
	/// compute overal legs positions
	for (int legno = 1; legno <= leg_count; ++legno) {
		QList<QPair<int, int>> relay_stime;
		for (int i = 0; i < relays.count(); ++i) {
			Relay &relay = relays[i];
			Leg &leg = relay.legs[legno - 1];
			if(!leg.notfinish && !leg.disq) {
				if(legno == 1)
					leg.stime = leg.time;
				else if(relay.legs[legno-2].stime > 0)
					leg.stime = leg.time + relay.legs[legno-2].stime;
			}
			if(leg.stime > 0)
				relay_stime << QPair<int, int>(relay.relayId, leg.stime);
		}
		std::sort(relay_stime.begin(), relay_stime.end(), [](const QPair<int, int> &a, const QPair<int, int> &b) {return a.second < b.second;});
		int pos = 0;
		for(const QPair<int, int> &p : relay_stime) {
			int relay_id = p.first;
			for (int i = 0; i < relays.count(); ++i) {
				if(relays[i].relayId == relay_id) {
					Relay &relay = relays[i];
					Leg &leg = relay.legs[legno - 1];
					leg.spos = ++pos;
					break;
				}
			}
		}
	}
	if(exclude_not_finish) {
		/*
		relays.erase(std::remove_if(relays.begin(),
									  relays.end(),
									  [](const Relay &r){return r.time(leg_count) == TIME_NOT_FINISH;}),
					   relays.end());
		*/
		QMutableListIterator<Relay> i(relays);
		while (i.hasNext()) {
			const Relay &r = i.next();
			if(r.time(leg_count) == qog::TimeMs::NOT_FINISH_TIME_MSEC)
				i.remove();
		}
	}
	/// sort relays
	std::sort(relays.begin(), relays.end(), [leg_count](const Relay &a, const Relay &b) {
		return a.time(leg_count) < b.time(leg_count);
	});

	int time0 = 0;
	qf::core::utils::TreeTable tt;
	tt.appendColumn("pos", QVariant::Int);
	tt.appendColumn("name", QVariant::String);
	tt.appendColumn("time", QVariant::Int);
	tt.appendColumn("loss", QVariant::Int);
	for (int i = 0; i < qMin(relays.count(), places); ++i) {
		qf::core::utils::TreeTableRow rr = tt.appendRow();
		Relay &relay = relays[i];
		int time = relay.time(leg_count);
		if(i == 0)
			time0 = time;
		int prev_time = (i > 0)? relays[i-1].time(leg_count): 0;
		rr.setValue("pos", (time <= qog::TimeMs::MAX_REAL_TIME_MSEC && time > prev_time)? i+1: 0);
		rr.setValue("name", relay.name);
		rr.setValue("time", time);
		rr.setValue("loss", (time <= qog::TimeMs::MAX_REAL_TIME_MSEC)?time - time0: 0);
		qf::core::utils::TreeTable tt2;
		tt.appendColumn("name", QVariant::String);
		tt.appendColumn("reg", QVariant::String);
		tt.appendColumn("time", QVariant::Int);
		tt.appendColumn("pos", QVariant::Int);
		tt.appendColumn("stime", QVariant::Int);
		tt.appendColumn("spos", QVariant::Int);
		//tt.appendColumn("disq", QVariant::Bool);
		for (int j = 0; j < relay.legs.count(); ++j) {
			Leg &leg = relay.legs[j];
			qf::core::utils::TreeTableRow rr2 = tt2.appendRow();
			rr2.setValue("competitorName", leg.name);
			rr2.setValue("registration", leg.reg);
			rr2.setValue("time",
						 leg.disq? qog::TimeMs::DISQ_TIME_MSEC
								: (leg.time == 0)? qog::TimeMs::NOT_FINISH_TIME_MSEC
												: leg.time);
			rr2.setValue("pos", leg.pos);
			rr2.setValue("stime", leg.stime);
			rr2.setValue("spos", leg.spos);
			//rr2.setValue("disq", leg.disq);
		}
		rr.appendTable(tt2);
	}
	//qfInfo() << tt.toString();
	return tt;
}

}
