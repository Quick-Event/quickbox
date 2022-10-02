#include "relaysplugin.h"
#include "relaydocument.h"
#include "relaywidget.h"
#include "relayswidget.h"

#include <quickevent/core/og/timems.h>
#include <quickevent/core/si/checkedcard.h>
#include <quickevent/core/utils.h>
#include <quickevent/core/resultstatus.h>

#include <qf/qmlwidgets/framework/mainwindow.h>
#include <qf/qmlwidgets/framework/dockwidget.h>
#include <qf/qmlwidgets/dialogs/dialog.h>
#include <qf/qmlwidgets/action.h>
#include <qf/qmlwidgets/menubar.h>

#include <qf/core/sql/connection.h>
#include <qf/core/utils/treetable.h>
#include <qf/core/utils/htmlutils.h>
#include <qf/core/model/sqltablemodel.h>
#include <qf/core/log.h>
#include <qf/core/assert.h>
#include <plugins/Event/src/eventplugin.h>
#include <plugins/Runs/src/runsplugin.h>

#include <QFile>
#include <QQmlEngine>

#include <qf/core/utils/timescope.h>

namespace qfw = qf::qmlwidgets;
namespace qff = qf::qmlwidgets::framework;
namespace qfd = qf::qmlwidgets::dialogs;
namespace qfm = qf::core::model;
namespace qfs = qf::core::sql;
namespace qog = quickevent::core::og;
using quickevent::gui::PartWidget;
using qff::getPlugin;
using Event::EventPlugin;
using Runs::RunsPlugin;

namespace Relays {

RelaysPlugin::RelaysPlugin(QObject *parent)
	: Super("Relays", parent)
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
	qff::initPluginWidget<RelaysWidget, PartWidget>(tr("&Relays"), featureId());

	connect(getPlugin<EventPlugin>(), &Event::EventPlugin::dbEventNotify, this, &RelaysPlugin::onDbEventNotify);

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
	Q_UNUSED(checked_card)
	qfLogFuncFrame();// << checked_card;
	/// recalculate team times

}

namespace {

struct Leg
{
	QString fullName;
	QString firstName;
	QString lastName;
	QString reg;
	int runId = 0;
	//int courseId = 0;
	int time = 0;
	int pos = 0;
	int stime = 0;
	int spos = 0;
	quickevent::core::ResultStatus resultStatus;
};

struct Relay
{
	QString name;
	QVector<Leg> legs;
	int relayNumber = 0;
	int relayId = 0;
	int loss = 0;

	int time(int leg_cnt) const
	{
		int ret = 0;
		for (int i = 0; i < qMin(legs.count(), leg_cnt); ++i) {
			const Leg &leg = legs[i];
			if (leg.resultStatus.isOk())
				ret += leg.time;
			else
				return leg.resultStatus.getOGTime(leg.time);
		}
		return ret;
	}
	QString status(int leg_cnt) const
	{
		for (int i = 0; i < qMin(legs.count(), leg_cnt); ++i) {
			const Leg &leg = legs[i];
			return leg.resultStatus.statusXml();
		}
		return QStringLiteral("DidNotStart");	// relay leg not found
	}
};
}

qf::core::utils::TreeTable RelaysPlugin::nLegsResultsTable(const QString &where_option, int leg_count, int places, bool exclude_not_finish)
{
	qfLogFuncFrame() << "leg cnt:" << leg_count;
	qf::core::utils::TreeTable tt;
	tt.setValue("event", getPlugin<EventPlugin>()->eventConfig()->value("event"));
	tt.setValue("stageStart", getPlugin<EventPlugin>()->stageStartDateTime(1));
	tt.appendColumn("className", QVariant::String);
	qfs::QueryBuilder qb;
	qb.select2("classes", "id, name")
			.from("classes")
			.orderBy("classes.name");
	if(!where_option.isEmpty()) {
		qb.where(where_option);
	}
	qfs::Query q;
	q.execThrow(qb.toString());
	while(q.next()) {
		int ix = tt.appendRow();
		qf::core::utils::TreeTableRow tt_row = tt.row(ix);
		tt_row.setValue("className", q.value("classes.name"));
		qf::core::utils::TreeTable tt2 = nLegsClassResultsTable(q.value("classes.id").toInt(), leg_count, places, exclude_not_finish);
		tt_row.appendTable(tt2);
		tt.setRow(ix, tt_row);
		//qfDebug() << tt2.toString();
	}
	auto wt = [tt]() {
		QFile f("/home/fanda/t/relays.json");
		f.open(QFile::WriteOnly);
		f.write(tt.toString().toUtf8());
		return f.fileName();
	};
	qfDebug() << "nLegsResultsTable table:" << wt();
	return tt;
}

qf::core::utils::TreeTable RelaysPlugin::nLegsClassResultsTable(int class_id, int leg_count, int max_places, bool exclude_not_finish)
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
			r.relayNumber = q.value("relays.number").toInt();
			r.name = (q.value("relays.number").toString()
					+ ' ' + q.value("relays.club").toString()
					+ ' ' + q.value("relays.name").toString()
					+ ' ' + q.value("clubs.name").toString()).trimmed();
			for (int i = 0; i < leg_count; ++i)
				r.legs << Leg();
			relays << r;
			qfDebug() << r.name;
			//relay_ids << QString::number(r.relayId);
		}
	}
	{
		qfs::QueryBuilder qb;
		qb.select2("competitors", "id, registration")
				.select2("runs", "id, relayId, leg")
				.select2("competitors", "firstName, lastName")
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
					leg.fullName = q.value("competitorName").toString();
					leg.firstName = q.value("firstName").toString();
					leg.lastName = q.value("lastName").toString();
					leg.runId = q.value("runs.id").toInt();
					leg.reg = q.value("competitors.registration").toString();
					//leg.courseId = getPlugin<RunsPlugin>()->courseForRun(leg.runId);
					break;
				}
			}
		}
	}
	for (int legno = 1; legno <= leg_count; ++legno) {
		qfs::QueryBuilder qb;
		qb.select2("runs", "id, relayId, timeMs," + quickevent::core::ResultStatus::dbRunsColumnList())
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
						leg.resultStatus.fillFromQuery(q);
						leg.time = q.value("timeMs").toInt();
						leg.pos = leg.resultStatus.isOk()? run_pos : 0;
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
			if(leg.resultStatus.isOk()) {
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
	tt.appendColumn("relayNumber", QVariant::Int);
	tt.appendColumn("id", QVariant::Int);
	tt.appendColumn("time", QVariant::Int);
	tt.appendColumn("loss", QVariant::Int);
	tt.appendColumn("status", QVariant::String);
	for (int i = 0; i < relays.count() && i < max_places; ++i) {
		int ix = tt.appendRow();
		qf::core::utils::TreeTableRow tt_row = tt.row(ix);
		const Relay &relay = relays[i];
		int time = relay.time(leg_count);
		if(i == 0)
			time0 = time;
		int prev_time = (i > 0)? relays[i-1].time(leg_count): 0;
		tt_row.setValue("pos", (time <= qog::TimeMs::MAX_REAL_TIME_MSEC && time > prev_time)? i+1: 0);
		tt_row.setValue("name", relay.name);
		tt_row.setValue("relayNumber", relay.relayNumber);
		tt_row.setValue("id", relay.relayId);
		tt_row.setValue("time", time);
		tt_row.setValue("loss", (time <= qog::TimeMs::MAX_REAL_TIME_MSEC)?time - time0: 0);
		tt_row.setValue("status", relay.status(relay.legs.count()));
		qfDebug() << tt.rowCount() << relay.name;
		qf::core::utils::TreeTable tt2;
		tt2.appendColumn("competitorName", QVariant::String);
		tt2.appendColumn("firstName", QVariant::String);
		tt2.appendColumn("lastName", QVariant::String);
		tt2.appendColumn("registration", QVariant::String);
		tt2.appendColumn("time", QVariant::Int);
		tt2.appendColumn("pos", QVariant::Int);
		tt2.appendColumn("status", QVariant::String);
		tt2.appendColumn("stime", QVariant::Int);
		tt2.appendColumn("spos", QVariant::Int);
		tt2.appendColumn("runId", QVariant::Int);
		tt2.appendColumn("courseId", QVariant::Int);
		tt2.appendColumn("sstatus", QVariant::String);
		for (int j = 0; j < relay.legs.count(); ++j) {
			const Leg &leg = relay.legs[j];
			int ix2 = tt2.appendRow();
			qf::core::utils::TreeTableRow tt2_row = tt2.row(ix2);
			tt2_row.setValue("competitorName", leg.fullName);
			tt2_row.setValue("firstName", leg.firstName);
			tt2_row.setValue("lastName", leg.lastName);
			tt2_row.setValue("registration", leg.reg);
			tt2_row.setValue("time", leg.resultStatus.getOGTime(leg.time));
			tt2_row.setValue("pos", leg.pos);
			tt2_row.setValue("status", leg.resultStatus.statusXml());
			tt2_row.setValue("stime", leg.stime);
			tt2_row.setValue("spos", leg.spos);
			tt2_row.setValue("runId", leg.runId);
			tt2_row.setValue("sstatus", relay.status(j+1));
			//tt2_row.setValue("courseId", leg.courseId);
			tt2.setRow(ix2, tt2_row);
			qfDebug() << '\t' << leg.pos << leg.fullName;
		}
		tt_row.appendTable(tt2);
		tt.setRow(ix, tt_row);
	}
	//qfInfo() << tt.toString();
	return tt;
}

QVariant RelaysPlugin::startListByClassesTableData(const QString &class_filter)
{
	qfLogFuncFrame() << class_filter;
	qf::core::model::SqlTableModel model;
	qf::core::model::SqlTableModel model2;
	{
		qf::core::sql::QueryBuilder qb;
		qb.select2("classes", "id, name")
			.from("classes")
			.orderBy("classes.name");//.limit(1);
		if(!class_filter.isEmpty()) {
			qb.where(class_filter);
		}
		model.setQueryBuilder(qb, true);
	}
	//console.info("currentStageTable query:", reportModel.effectiveQuery());
	model.reload();
	qf::core::utils::TreeTable tt = model.toTreeTable();
	tt.setValue("event", getPlugin<EventPlugin>()->eventConfig()->value("event"));
	tt.setValue("stageStart", getPlugin<EventPlugin>()->stageStartDateTime(1));
	{
		qf::core::sql::QueryBuilder qb;
		qb.select2("relays", "id, name, number")
				.select2("clubs", "name")
				.select("clubs.abbr, clubs.name, COALESCE(relays.club, '') || ' ' || COALESCE(relays.name, '') AS relayName")
				.from("relays")
				.join("relays.club", "clubs.abbr")
				.where("relays.classId={{class_id}}")
				.orderBy("relays.number, relayName");
		model.setQueryBuilder(qb, true);
	}
	{
		qf::core::sql::QueryBuilder qb;
		qb.select2("competitors", "registration")
			.select("competitors.firstName, competitors.lastName, COALESCE(competitors.lastName, '') || ' ' || COALESCE(competitors.firstName, '') AS competitorName")
			.select2("runs", "leg, siId, startTimeMs")
			.from("runs")
			.join("runs.competitorId", "competitors.id")
			.join("runs.relayId", "relays.id")
			.where("runs.relayId={{relay_id}}")
			.where("runs.isRunning")
			.orderBy("runs.leg");
		model2.setQueryBuilder(qb, true);
	}
	for(int i=0; i<tt.rowCount(); i++) {
		int class_id = tt.row(i).value("classes.id").toInt();
		//console.debug("class id:", class_id);
		QVariantMap qm;
		qm["class_id"] = class_id;
		model.setQueryParameters(qm);
		model.reload();
		qf::core::utils::TreeTable tt2 = model.toTreeTable();
		for (int j = 0; j < tt2.rowCount(); ++j) {
			int relay_id = tt2.row(j).value("relays.id").toInt();
			QVariantMap qm2;
			qm2["relay_id"] = relay_id;
			model2.setQueryParameters(qm2);
			model2.reload();
			qf::core::utils::TreeTable tt3 = model2.toTreeTable();
			tt2.appendTable(j, tt3);
		}
		tt.appendTable(i, tt2);
	}
	//qfInfo() << tt.toString();
	return tt.toVariant();
}

static void append_list(QVariantList &lst, const QVariantList &new_lst)
{
	lst.insert(lst.count(), new_lst);
}

static QString datetime_to_string(const QDateTime &dt)
{
	return quickevent::core::Utils::dateTimeToIsoStringWithUtcOffset(dt);
}

QString RelaysPlugin::resultsIofXml30()
{
	QDateTime start00 = getPlugin<EventPlugin>()->stageStartDateTime(1);
	qfDebug() << "creating table";
	//auto tt_classes = getPlugin<RelaysPlugin>()->nLegsResultsTable("classes.name='D105'", 999, 999999, false);
	auto tt_classes = getPlugin<RelaysPlugin>()->nLegsResultsTable(QString(), 999, 999999, false);
	QVariantList result_list{
		"ResultList",
		QVariantMap{
			{"xmlns", "http://www.orienteering.org/datastandard/3.0"},
			{"status", "Complete"},
			{"iofVersion", "3.0"},
			{"creator", "QuickEvent"},
			{"createTime", datetime_to_string(QDateTime::currentDateTime())},
		}
	};
	{
		QVariantList event_lst{"Event"};
		QVariantMap event = tt_classes.value("event").toMap();
		event_lst.insert(event_lst.count(), QVariantList{"Id", QVariantMap{{"type", "ORIS"}}, event.value("importId")});
		event_lst.insert(event_lst.count(), QVariantList{"Name", event.value("name")});
		event_lst.insert(event_lst.count(), QVariantList{"StartTime",
				   QVariantList{"Date", event.value("date")},
				   QVariantList{"Time", event.value("time")}
		});
		event_lst.insert(event_lst.count(),
			QVariantList{"Official",
				QVariantMap{{"type", "director"}},
				QVariantList{"Person",
					QVariantList{"Name",
						QVariantList{"Family", event.value("director").toString().section(' ', 1, 1)},
						QVariantList{"Given", event.value("director").toString().section(' ', 0, 0)},
					}
				},
			}
		);
		event_lst.insert(event_lst.count(),
			QVariantList{"Official",
				QVariantMap{{"type", "mainReferee"}},
				QVariantList{"Person",
					QVariantList{"Name",
						QVariantList{"Family", event.value("mainReferee").toString().section(' ', 1, 1)},
						QVariantList{"Given", event.value("mainReferee").toString().section(' ', 0, 0)},
					}
				},
			}
		);
		result_list.insert(result_list.count(), event_lst);
	}
	for(int i=0; i<tt_classes.rowCount(); i++) {

		QVariantList class_result{"ClassResult"};
		const qf::core::utils::TreeTableRow tt_classes_row = tt_classes.row(i);
		QF_TIME_SCOPE("exporting class: " + tt_classes_row.value(QStringLiteral("className")).toString());
		append_list(class_result,
			QVariantList{"Class",
				QVariantList{"Name", tt_classes_row.value(QStringLiteral("className")) },
			}
		);
		qf::core::utils::TreeTable tt_teams = tt_classes_row.table();
		for(int j=0; j<tt_teams.rowCount(); j++) {
			QVariantList team_result{"TeamResult"};
			const qf::core::utils::TreeTableRow tt_teams_row = tt_teams.row(j);
			QF_TIME_SCOPE("exporting team: " + tt_teams_row.value(QStringLiteral("name")).toString());
			append_list(team_result,
				QVariantList{"Name", tt_teams_row.value(QStringLiteral("name")) }
			);
			int relay_number = tt_teams_row.value(QStringLiteral("relayNumber")).toInt();
			append_list(team_result,
				QVariantList{"BibNumber", relay_number }
			);

			qf::core::utils::TreeTable tt_legs = tt_teams_row.table();
			for (int k = 0; k < tt_legs.rowCount(); ++k) {
				int leg = k + 1;
				QF_TIME_SCOPE("exporting leg: " + QString::number(leg));
				const qf::core::utils::TreeTableRow tt_leg_row = tt_legs.row(k);
				QVariantList member_result{"TeamMemberResult"};
				append_list(member_result,
							QVariantList{"Person",
								QVariantList{"Id", QVariantMap{{"type", "CZE"}}, tt_leg_row.value(QStringLiteral("registration"))},
								QVariantList{"Name",
								   QVariantList{"Family", tt_leg_row.value(QStringLiteral("lastName"))},
								   QVariantList{"Given", tt_leg_row.value(QStringLiteral("firstName"))},
								}
							} );
				QVariantList person_result{"Result"};
				append_list(person_result, QVariantList{"Leg", k+1 } );
				append_list(person_result, QVariantList{"BibNumber", QString::number(relay_number) + '.' + QString::number(k+1)});
				int run_id = tt_leg_row.value(QStringLiteral("runId")).toInt();
				int stime = 0, ftime = 0, time_msec = 0, siId = 0;
				if(run_id > 0) {
					qfs::QueryBuilder qb;
					qb.select2("runs", "startTimeMs, finishTimeMs, timeMs, siId")
							.from("runs").where("id=" + QString::number(run_id));
					qfs::Query q;
					q.execThrow(qb.toString());
					if(q.next()) {
						stime = q.value(0).toInt();
						ftime = q.value(1).toInt();
						time_msec = q.value(2).toInt();
						siId = q.value(3).toInt();
					}
					else {
						qfWarning() << "Cannot load run for id:" << run_id;
					}
				}
				append_list(person_result, QVariantList{"StartTime", datetime_to_string(start00.addMSecs(stime))});
				append_list(person_result, QVariantList{"FinishTime", datetime_to_string(start00.addMSecs(ftime))});
				append_list(person_result, QVariantList{"Time", time_msec / 1000});
				 // MISSING TimeBehind
				append_list(person_result, QVariantList{"Position", QVariantMap{{"type", "Leg"}}, tt_leg_row.value(QStringLiteral("pos"))});
				// MISSING position course append_list(person_result, QVariantList{"Position", QVariantMap{{"type", "course"}}, tt_laps_row.value(QStringLiteral("pos"))});
				append_list(person_result, QVariantList{"Status", tt_leg_row.value(QStringLiteral("status"))});
				QVariantList overall_result{"OverallResult"};
				{
					append_list(overall_result, QVariantList{"Time", tt_leg_row.value(QStringLiteral("stime")).toInt() / 1000});
					append_list(overall_result, QVariantList{"Position", tt_leg_row.value(QStringLiteral("spos"))});
					append_list(overall_result, QVariantList{"Status", tt_leg_row.value(QStringLiteral("sstatus"))});
					 // MISSING TimeBehind
				}
				append_list(person_result, overall_result);
				int course_id = getPlugin<RunsPlugin>()->courseForRelay(relay_number, leg);
				{
					QF_TIME_SCOPE("exporting course: " + QString::number(course_id));
					QVariantList course{"Course"};
					append_list(course, QVariantList{"Id", course_id});
					{
						qfs::QueryBuilder qb;
						qb.select2("courses", "name, length, climb")
								.from("courses").where("id=" + QString::number(course_id));
						qfs::Query q;
						q.execThrow(qb.toString());
						if(q.next()) {
							append_list(course, QVariantList{"Name", q.value(0)});
							append_list(course, QVariantList{"Length", q.value(1)});
							append_list(course, QVariantList{"Climb", q.value(2)});
						}
						else {
							qfWarning() << "Cannot load course for id:" << course_id;
						}
					}
					append_list(person_result, course);
				}
				{
					QF_TIME_SCOPE("exporting laps");
					qf::core::sql::QueryBuilder qb;
					qb.select2("runlaps", "position, stpTimeMs")
							.from("runlaps").where("runId=" + QString::number(run_id))
							.where("code >= " + QString::number(quickevent::core::CodeDef::PUNCH_CODE_MIN))
							.where("code <= " + QString::number(quickevent::core::CodeDef::PUNCH_CODE_MAX))
							.orderBy("position") ;
					//qfInfo() << qb.toString();
					auto q = qf::core::sql::Query::fromExec(qb.toString());
					quickevent::core::CourseDef csd = getPlugin<RunsPlugin>()->courseForCourseId(course_id);
					QVariantList codes = csd.codes();
					int sql_pos = -1;
					for (int ix = 0; ix < codes.count(); ++ix) {
						quickevent::core::CodeDef cd(codes[ix].toMap());
						int pos = ix + 1;
						if(sql_pos < 0) {
							if(q.next()) {
								sql_pos = q.value(0).toInt();
							}
						}
						int time = 0;
						if(pos == sql_pos) {
							sql_pos = -1;
							time = q.value(1).toInt();
						}
						QVariantList split{QStringLiteral("SplitTime")};
						append_list(split, QVariantList{"ControlCode", cd.code()});
						if(time == 0)
							split.insert(1, QVariantMap{ {QStringLiteral("status"), QStringLiteral("Missing")} });
						else
							append_list(split, QVariantList{"Time", time / 1000});
						append_list(person_result, split);
					}
				}
				append_list(person_result, QVariantList{"ControlCard", siId});
				append_list(member_result, person_result);
				append_list(team_result, member_result);
			}
			append_list(class_result, team_result);
		}
		append_list(result_list, class_result);
	}
	qf::core::utils::HtmlUtils::FromXmlListOptions opts;
	opts.setDocumentTitle(tr("Relays IOF-XML 3.0 results"));
	return qf::core::utils::HtmlUtils::fromXmlList(result_list, opts);
}

QString RelaysPlugin::startListIofXml30()
{
	QDateTime start00 = getPlugin<EventPlugin>()->stageStartDateTime(1);
	qfDebug() << "creating table";
	//auto tt_classes = getPlugin<RelaysPlugin>()->nLegsResultsTable("classes.name='D105'", 999, 999999, false);
	qf::core::utils::TreeTable tt_classes = startListByClassesTableData(QString());
	QVariantList start_list{
		"StartList",
		QVariantMap{
			{"xmlns", "http://www.orienteering.org/datastandard/3.0"},
			{"status", "Complete"},
			{"iofVersion", "3.0"},
			{"creator", "QuickEvent"},
			{"createTime", datetime_to_string(QDateTime::currentDateTime())},
		}
	};
	{
		QVariantList event_lst{"Event"};
		QVariantMap event = tt_classes.value("event").toMap();
		event_lst.insert(event_lst.count(), QVariantList{"Id", QVariantMap{{"type", "ORIS"}}, event.value("importId")});
		event_lst.insert(event_lst.count(), QVariantList{"Name", event.value("name")});
		event_lst.insert(event_lst.count(), QVariantList{"StartTime",
				   QVariantList{"Date", event.value("date")},
				   QVariantList{"Time", event.value("time")}
		});
		event_lst.insert(event_lst.count(),
			QVariantList{"Official",
				QVariantMap{{"type", "director"}},
				QVariantList{"Person",
					QVariantList{"Name",
						QVariantList{"Family", event.value("director").toString().section(' ', 1, 1)},
						QVariantList{"Given", event.value("director").toString().section(' ', 0, 0)},
					}
				},
			}
		);
		event_lst.insert(event_lst.count(),
			QVariantList{"Official",
				QVariantMap{{"type", "mainReferee"}},
				QVariantList{"Person",
					QVariantList{"Name",
						QVariantList{"Family", event.value("mainReferee").toString().section(' ', 1, 1)},
						QVariantList{"Given", event.value("mainReferee").toString().section(' ', 0, 0)},
					}
				},
			}
		);
		start_list.insert(start_list.count(), event_lst);
	}
	for(int i=0; i<tt_classes.rowCount(); i++) {
		QVariantList class_start{"ClassStart"};
		const qf::core::utils::TreeTableRow tt_classes_row = tt_classes.row(i);
		QF_TIME_SCOPE("exporting class: " + tt_classes_row.value(QStringLiteral("classes.name")).toString());
		append_list(class_start,
			QVariantList{"Class",
				QVariantList{"Name", tt_classes_row.value(QStringLiteral("classes.name")) },
			}
		);
		qf::core::utils::TreeTable tt_teams = tt_classes_row.table();
		for(int j=0; j<tt_teams.rowCount(); j++) {
			QVariantList team_start{"TeamStart"};
			const qf::core::utils::TreeTableRow tt_teams_row = tt_teams.row(j);
			QF_TIME_SCOPE("exporting team: " + tt_teams_row.value(QStringLiteral("relayName")).toString());
			append_list(team_start,
				QVariantList{"Name", tt_teams_row.value(QStringLiteral("relayName")) }
			);

			append_list(team_start,
				QVariantList{"Organisation",
					QVariantList{"ShortName", tt_teams_row.value(QStringLiteral("abbr"))},
					QVariantList{"Name", tt_teams_row.value(QStringLiteral("clubs.name"))}
				}
			);

			int relay_number = tt_teams_row.value(QStringLiteral("relays.number")).toInt();
			append_list(team_start,
				QVariantList{"BibNumber", relay_number }
			);

			qf::core::utils::TreeTable tt_legs = tt_teams_row.table();
			for (int k = 0; k < tt_legs.rowCount(); ++k) {
				int leg = k + 1;
				QF_TIME_SCOPE("exporting leg: " + QString::number(leg));
				const qf::core::utils::TreeTableRow tt_leg_row = tt_legs.row(k);
				QVariantList member_start{"TeamMemberStart"};
				append_list(member_start,
							QVariantList{"Person",
								QVariantList{"Id", QVariantMap{{"type", "CZE"}}, tt_leg_row.value(QStringLiteral("registration"))},
								QVariantList{"Name",
								   QVariantList{"Family", tt_leg_row.value(QStringLiteral("lastName"))},
								   QVariantList{"Given", tt_leg_row.value(QStringLiteral("firstName"))},
								}
							} );
				QVariantList start{"Start"};
				append_list(start, QVariantList{"Leg", k+1 } );
				append_list(start, QVariantList{"BibNumber", QString::number(relay_number) + '.' + QString::number(k+1)});
				append_list(start, QVariantList{"StartTime", datetime_to_string(start00.addMSecs(tt_leg_row.value(QStringLiteral("runs.startTimeMs")).toInt()))});
				int course_id = getPlugin<RunsPlugin>()->courseForRelay(relay_number, leg);
				{
					QF_TIME_SCOPE("exporting course: " + QString::number(course_id));
					QVariantList course{"Course"};
					append_list(course, QVariantList{"Id", course_id});
					{
						qfs::QueryBuilder qb;
						qb.select2("courses", "name, length, climb")
								.from("courses").where("id=" + QString::number(course_id));
						qfs::Query q;
						q.execThrow(qb.toString());
						if(q.next()) {
							append_list(course, QVariantList{"Name", q.value(0)});
							append_list(course, QVariantList{"Length", q.value(1)});
							append_list(course, QVariantList{"Climb", q.value(2)});
						}
						else {
							qfWarning() << "Cannot load course for id:" << course_id;
						}
					}
					append_list(start, course);
				}

				QVariant siId = tt_leg_row.value(QStringLiteral("runs.siId"));
				if (siId.toBool()) {
					append_list(start, QVariantList{"ControlCard", siId.toInt()});
				}
				append_list(member_start, start);
				append_list(team_start, member_start);
			}
			append_list(class_start, team_start);
		}
		append_list(start_list, class_start);
	}
	qf::core::utils::HtmlUtils::FromXmlListOptions opts;
	opts.setDocumentTitle(tr("Relays IOF-XML 3.0 results"));
	return qf::core::utils::HtmlUtils::fromXmlList(start_list, opts);
}

}
