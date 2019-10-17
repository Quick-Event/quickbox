#include "cardreaderplugin.h"
#include "cardchecker.h"
#include "../cardreaderpartwidget.h"
#include "../services/racomclient.h"

#include <Event/eventplugin.h>
#include <Runs/runsplugin.h>

#include <quickevent/core/og/timems.h>
#include <quickevent/core/si/punchrecord.h>
#include <quickevent/core/si/checkedcard.h>
#include <quickevent/core/si/readcard.h>

#include <qf/qmlwidgets/framework/mainwindow.h>

#include <qf/core/log.h>
#include <qf/core/assert.h>
#include <qf/core/exception.h>
#include <qf/core/sql/query.h>
#include <qf/core/sql/connection.h>
#include <qf/core/sql/transaction.h>
#include <qf/core/sql/querybuilder.h>

#include <QJSValue>
#include <QMetaObject>
#include <QSqlRecord>

//#define QF_TIMESCOPE_ENABLED
#include <qf/core/utils/timescope.h>

namespace qff = qf::qmlwidgets::framework;
namespace qfs = qf::core::sql;

namespace CardReader {

static Event::EventPlugin* eventPlugin()
{
	qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
	auto *plugin = qobject_cast<Event::EventPlugin*>(fwk->plugin("Event"));
	QF_ASSERT_EX(plugin != nullptr, "Bad Event plugin!");
	return plugin;
}

static Runs::RunsPlugin *runsPlugin()
{
	qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
	auto *plugin = qobject_cast<Runs::RunsPlugin *>(fwk->plugin("Runs"));
	QF_ASSERT_EX(plugin != nullptr, "Bad plugin");
	return plugin;
}

const QLatin1String CardReaderPlugin::SETTINGS_PREFIX("plugins/CardReader");
const int CardReaderPlugin::FINISH_PUNCH_POS = quickevent::core::si::PunchRecord::FINISH_PUNCH_CODE;

CardReaderPlugin::CardReaderPlugin(QObject *parent)
	: Super(parent)
{
	connect(this, &CardReaderPlugin::installed, this, &CardReaderPlugin::onInstalled);
}

QString CardReaderPlugin::settingsPrefix()
{
	static const QString s = CardReaderPlugin::SETTINGS_PREFIX;
	return s;
}

void CardReaderPlugin::onInstalled()
{
	qff::MainWindow *fwk = qff::MainWindow::frameWork();
	CardReaderPartWidget *pw = new CardReaderPartWidget(manifest()->featureId());
	fwk->addPartWidget(pw);

	services::RacomClient *racom_client = new services::RacomClient(this);
	services::Service::addService(racom_client);
}

QQmlListProperty<CardReader::CardChecker> CardReaderPlugin::cardCheckersListProperty()
{
	/// Generally this constructor should not be used in production code, as a writable QList violates QML's memory management rules.
	/// However, this constructor can be very useful while prototyping.
	return QQmlListProperty<CardReader::CardChecker>(this, m_cardCheckers);
}

CardChecker *CardReaderPlugin::currentCardChecker()
{
	auto ret = m_cardCheckers.value(currentCardCheckerIndex());
	QF_ASSERT(ret != nullptr, QString("No card checker for index %1 installed").arg(currentCardCheckerIndex()), return nullptr);
	return ret;
}

int CardReaderPlugin::currentStageId()
{
	qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
	auto event_plugin = qobject_cast<Event::EventPlugin *>(fwk->plugin("Event"));
	QF_ASSERT(event_plugin != nullptr, "Bad plugin", return 0);
	int ret = event_plugin->currentStageId();
	return ret;
}

int CardReaderPlugin::findRunId(int si_id, int si_finish_time, QString *err_msg)
{
	int ret = 0;
	int row_cnt = 0;
	int last_id = 0;
	//int start_time_msec = eventPlugin()->msecToStageStartAM(si_start_time);
	int finish_time_msec = eventPlugin()->msecToStageStartAM(si_finish_time);
	bool is_relays = eventPlugin()->eventConfig()->isRelays();
	if(is_relays) {
		qf::core::sql::Query q;
		q.exec("SELECT id, leg, startTimeMs, finishTimeMs FROM runs WHERE siId=" QF_IARG(si_id)
			   " AND isRunning"
			   " ORDER BY leg DESC"
			   , qf::core::Exception::Throw);
		while(q.next()) {
			row_cnt++;
			last_id = q.value("id").toInt();
			if(finish_time_msec == quickevent::core::og::TimeMs::UNREAL_TIME_MSEC)
				continue; /// skip all checks when finish time is not known
			int st = q.value("startTimeMs").toInt();
			int leg = q.value("leg").toInt();
			if(st == 0 && leg != 1) {
				/// start time not set => this leg does not event start
				continue;
			}
			if(st >= finish_time_msec) {
				/// start in future, this run cannot have this siid
				continue;
			}
			int ft = q.value("finishTimeMs").toInt();
			if(ft == finish_time_msec)
				qfInfo() << "Multiple reads of SI:" << si_id;
			else if(ft > 0)
				qfWarning() << "Multiple reads of SI:" << si_id << "with different finish time";
			ret = last_id;
			break;
		}
	}
	else {
		int stage_no = currentStageId();
		qf::core::sql::Query q;
		q.exec("SELECT id, startTimeMs, finishTimeMs FROM runs WHERE stageId=" QF_IARG(stage_no)
			   " AND siId=" QF_IARG(si_id)
			   " AND isRunning"
			   //" ORDER BY finishTimeMs"
			   , qf::core::Exception::Throw);
		while(q.next()) {
			row_cnt++;
			last_id = q.value("id").toInt();
			if(finish_time_msec == quickevent::core::og::TimeMs::UNREAL_TIME_MSEC)
				continue; /// skip all checks when finish time is not known
			int st = q.value("startTimeMs").toInt();
			//int ft = q.value("finishTimeMs").toInt();
			if(st > finish_time_msec) {
				/// start in future, this run cannot have this siid
				continue;
			}
			if(ret == 0) {
				ret = last_id;
			}
			else {
				/// second possible run, give it up
				qfWarning() << "There are more runs with si:" << si_id << "run id1:" << ret << "id2:" << q.value("id").toInt();
				if(err_msg)
					*err_msg = tr("More runs with si: %1, run1 id: %2, run2 id: %3").arg(si_id).arg(ret).arg(q.value("id").toInt());
				ret = 0;
				break;
			}
		}
	}
	if(row_cnt == 1) {
		/// if we have just one record, skip all the checks
		ret = last_id;
	}
	if(ret == 0) {
		if(err_msg && err_msg->isEmpty())
			*err_msg = tr("Cannot find runs with si: %1").arg(si_id);
	}
	else {
		if(err_msg)
			*err_msg = QString();
	}
	return ret;
}

bool CardReaderPlugin::isCardLent(int si_id, int si_finish_time, int run_id)
{
	bool card_lent = false;
	bool card_returned = false;
	if(run_id == 0)
		run_id = findRunId(si_id, si_finish_time);
	qf::core::sql::Query q;
	if(run_id > 0) {
		q.exec("SELECT cardLent, cardReturned FROM runs WHERE id=" QF_IARG(run_id) );
		if(q.next()) {
			card_lent = q.value(0).toBool();
			card_returned = q.value(1).toBool();
		}
	}
	if(!card_lent && !card_returned) {
		q.exec("SELECT siId FROM lentcards WHERE NOT ignored AND siid=" QF_IARG(si_id) );
		if(q.next())
			card_lent = true;
	}
	return (card_lent && !card_returned);
}

quickevent::core::si::ReadCard CardReaderPlugin::readCard(int card_id)
{
	qfLogFuncFrame() << "card id:" << card_id;
	qf::core::sql::Query q;
	if(q.exec("SELECT * FROM cards WHERE id=" QF_IARG(card_id))) {
		if(q.next()) {
			quickevent::core::si::ReadCard rc(q.record());
			return rc;
		}
	}
	qfWarning() << "Cannot find card record for id:" << card_id;
	return quickevent::core::si::ReadCard();
}

quickevent::core::si::CheckedCard CardReaderPlugin::checkCard(int card_id, int run_id)
{
	qfLogFuncFrame() << "run id:" << run_id << "card id:" << card_id;
	QF_TIME_SCOPE("checkCard()");
	quickevent::core::si::ReadCard rc = readCard(card_id);
	if(!rc.isEmpty()) {
		if(run_id > 0)
			rc.setRunId(run_id);
		return checkCard(rc);
	}
	return quickevent::core::si::CheckedCard();
}

quickevent::core::si::CheckedCard CardReaderPlugin::checkCard(const quickevent::core::si::ReadCard &read_card)
{
	qfLogFuncFrame() << "run id:" << read_card.runId();
	QF_TIME_SCOPE("checkCard()");
	quickevent::core::si::CheckedCard cc;;
	CardReader::CardChecker *chk = currentCardChecker();
	QF_ASSERT(chk != nullptr, "CardChecker is NULL", return quickevent::core::si::CheckedCard());
	CardReader::CardCheckerCpp *cpp_chk = qobject_cast<CardReader::CardCheckerCpp*>(chk);
	if(cpp_chk) {
		cc = cpp_chk->checkCard(read_card);
	}
	else {
		QVariant ret_val;
		QMetaObject::invokeMethod(chk, "checkCard", Qt::DirectConnection,
								  Q_RETURN_ARG(QVariant, ret_val),
								  Q_ARG(QVariant, read_card));
		QJSValue jsv = ret_val.value<QJSValue>();
		QVariant v = jsv.toVariant();
		QVariantMap m = v.toMap();
		cc = quickevent::core::si::CheckedCard(m);
	}
	cc.setRunId(read_card.runId());
	cc.setCardNumber(read_card.cardNumber());
	qfDebug() << cc.toString();
	return cc;
}

int CardReaderPlugin::saveCardToSql(const quickevent::core::si::ReadCard &read_card)
{
	int ret = 0;
	QStringList punches;
	for(auto v : read_card.punches()) {
		quickevent::core::si::ReadPunch p(v.toMap());
		punches << p.toJsonArrayString();
	}
	qf::core::sql::Query q;
	q.prepare(QStringLiteral("INSERT INTO cards (stationNumber, siId, checkTime, startTime, finishTime, punches, runId, stageId, readerConnectionId, runIdAssignError)"
							 " VALUES (:stationNumber, :siId, :checkTime, :startTime, :finishTime, :punches, :runId, :stageId, :readerConnectionId, :runIdAssignError)")
			  , qf::core::Exception::Throw);
	q.bindValue(QStringLiteral(":stationNumber"), read_card.stationNumber());
	q.bindValue(QStringLiteral(":siId"), read_card.cardNumber());
	q.bindValue(QStringLiteral(":checkTime"), read_card.checkTime());
	q.bindValue(QStringLiteral(":startTime"), read_card.startTime());
	q.bindValue(QStringLiteral(":finishTime"), read_card.finishTime());
	q.bindValue(QStringLiteral(":punches"), '[' + punches.join(", ") + ']');
	q.bindValue(QStringLiteral(":runId"), read_card.runId());
	q.bindValue(QStringLiteral(":stageId"), currentStageId());
	q.bindValue(QStringLiteral(":readerConnectionId"), qf::core::sql::Connection::defaultConnection().connectionId());
	q.bindValue(QStringLiteral(":runIdAssignError"), read_card.runIdAssignError());
	if(q.exec()) {
		ret = q.lastInsertId().toInt();
	}
	else {
		qfError() << tr("Save card ERROR: %1").arg(q.lastErrorText());
	}
	return ret;
}

int CardReaderPlugin::savePunchRecordToSql(const quickevent::core::si::PunchRecord &punch_record)
{
	//qfInfo() << "PUNCH:" << punch_record.toString();
	int ret = 0;
	quickevent::core::si::PunchRecord punch = punch_record;
	punch.setstageid(currentStageId());

	Event::EventPlugin *event_plugin = eventPlugin();
	int time_msec = event_plugin->msecToStageStartAM(punch_record.time(), punch_record.msec());
	punch.settimems(time_msec);
	qf::core::sql::Query q;
	int run_id = punch.runid();
	if(run_id > 0) {
		q.exec("SELECT startTimeMs FROM runs WHERE id=" QF_IARG(run_id), qf::core::Exception::Throw);
		if(q.next()) {
			QVariant v = q.value(0);
			if(!v.isNull()) {
				punch.setruntimems(time_msec - v.toInt());
			}
		}
	}

	int code = resolveAltCode(punch.code(), punch.stageid());

	q.prepare(QStringLiteral("INSERT INTO punches (siId, code, time, msec, runId, stageId, timeMs, runTimeMs, marking)"
							 " VALUES (:siId, :code, :time, :msec, :runId, :stageId, :timeMs, :runTimeMs, :marking)")
							, qf::core::Exception::Throw);
	q.bindValue(QStringLiteral(":siId"), punch.siid());
	q.bindValue(QStringLiteral(":code"), code);
	q.bindValue(QStringLiteral(":time"), punch.time());
	q.bindValue(QStringLiteral(":msec"), punch.msec());
	q.bindValue(QStringLiteral(":runId"), punch.runid());
	q.bindValue(QStringLiteral(":stageId"), punch.stageid());
	q.bindValue(QStringLiteral(":marking"), punch.marking());
	q.bindValue(QStringLiteral(":timeMs"), punch.timems());
	q.bindValue(QStringLiteral(":runTimeMs"), punch.runtimems_isset()? punch.runtimems(): QVariant());
	/// it is not possible to save punch time as date-time to be independent on start00 since it depends on start00 due to 12H time format
	if(q.exec()) {
		ret = q.lastInsertId().toInt();
	}
	else {
		qfError() << tr("Save punch record ERROR: %1").arg(q.lastErrorText());
	}
	return ret;
}

void CardReaderPlugin::updateCheckedCardValuesSql(const quickevent::core::si::CheckedCard &checked_card) noexcept(false)
{
	QF_TIME_SCOPE("updateCheckedCardValuesSql()");
	int run_id = checked_card.runId();
	if(run_id <= 0)
		QF_EXCEPTION("Card doesn't contain runId information!");
	auto cc = qf::core::sql::Connection::forName();
	qf::core::sql::Query q(cc);
	{
		QF_TIME_SCOPE("DELETE FROM runlaps");
		q.exec("DELETE FROM runlaps WHERE runId=" + QString::number(run_id), qf::core::Exception::Throw);
	}
	q.prepare(QStringLiteral("INSERT INTO runlaps (runId, position, code, stpTimeMs, lapTimeMs) VALUES (:runId, :position, :code, :stpTimeMs, :lapTimeMs)"), qf::core::Exception::Throw);
	auto punch_list = checked_card.punches();
	if(punch_list.count()) {
		QF_TIME_SCOPE("INSERT INTO runlaps, records cnt: " + QString::number(punch_list.count()));
		int position = 0;
		for(auto v : punch_list) {
			position++;
			quickevent::core::si::CheckedPunch cp(v.toMap());
			//qfInfo() << run_id << position << cp;
			if(cp.stpTimeMs() > 0) {
				q.bindValue(QStringLiteral(":runId"), run_id);
				q.bindValue(QStringLiteral(":code"), cp.code());
				q.bindValue(QStringLiteral(":position"), position);
				q.bindValue(QStringLiteral(":stpTimeMs"), cp.stpTimeMs());
				q.bindValue(QStringLiteral(":lapTimeMs"), cp.lapTimeMs());
				q.exec(qf::core::Exception::Throw);
			}
		}
	}
	q.prepare("UPDATE runs SET checkTimeMs=:checkTimeMs, timeMs=:timeMs, finishTimeMs=:finishTimeMs, misPunch=:misPunch, badCheck=:badCheck, disqualified=:disqualified WHERE id=" + QString::number(run_id), qf::core::Exception::Throw);
	q.bindValue(QStringLiteral(":checkTimeMs"), checked_card.checkTimeMs());
	q.bindValue(QStringLiteral(":timeMs"), checked_card.timeMs());
	q.bindValue(QStringLiteral(":finishTimeMs"), checked_card.finishTimeMs());
	q.bindValue(QStringLiteral(":misPunch"), checked_card.isMisPunch());
	q.bindValue(QStringLiteral(":badCheck"), checked_card.isBadCheck());
	q.bindValue(QStringLiteral(":disqualified"), !checked_card.isOk());
	q.exec(qf::core::Exception::Throw);
	if(q.numRowsAffected() != 1)
		QF_EXCEPTION("Update runs error!");
	/*
	bool is_relays = eventPlugin()->eventConfig()->isRelays();
	if(is_relays) {
		/// set start time for next leg
		q.execThrow("SELECT relayId, leg FROM runs WHERE id=" + QString::number(run_id));
		if(q.next()) {
			int relay_id = q.value(0).toInt();
			int leg = q.value(1).toInt();
			q.exec("UPDATE runs SET startTimeMs=" + QString::number(checked_card.finishTimeMs())
				   + " WHERE relayId=" + QString::number(relay_id)
				   + " AND leg=" + QString::number(leg+1)
				   + " AND COALESCE(startTimeMs, 0)=0"
				   , qf::core::Exception::Throw);
		}
		else {
			qfError() << "run should be loaded, id:" << run_id;
		}
	}
	*/
}

bool CardReaderPlugin::saveCardAssignedRunnerIdSql(int card_id, int run_id)
{
	QF_TIME_SCOPE("saveCardAssignedRunnerIdSql()");
	auto cc = qf::core::sql::Connection::forName();
	qf::core::sql::Query q(cc);
	QString now = QStringLiteral("now()");
	if(cc.driverName().endsWith("SQLITE", Qt::CaseInsensitive))
		now = QStringLiteral("CURRENT_TIMESTAMP");
	bool ret = q.exec("UPDATE cards SET runId=" QF_IARG(run_id) ", runIdAssignTS=" + now + " WHERE id=" QF_IARG(card_id), !qf::core::Exception::Throw);
	return ret;
}

bool CardReaderPlugin::reloadTimesFromCard(int card_id, int run_id)
{
	qfLogFuncFrame() << "card id:" << run_id;
	QF_TIME_SCOPE("reloadTimesFromCard()");
	if(card_id == 0)
		return false;
	if(run_id == 0) {
		qf::core::sql::Query q;
		if(q.exec("SELECT runId FROM cards WHERE id=" QF_IARG(card_id))) {
			if(q.next()) {
				run_id = q.value(0).toInt();
			}
			else {
				qfWarning() << "Cannot find card record for id:" << card_id;
			}
		}
	}
	if(run_id == 0) {
		qfWarning() << "Cannot find runs id for card id:" << card_id;
		return false;
	}
	try {
		qf::core::sql::Transaction transaction;
		processCardToRunAssignment(card_id, run_id);
		transaction.commit();
		return true;
	}
	catch (const qf::core::Exception &e) {
		qfError() << "reloadTimesFromCard ERROR:" << e.message();
	}
	return false;
}

void CardReaderPlugin::assignCardToRun(int card_id, int run_id)
{
	saveCardAssignedRunnerIdSql(card_id, run_id);
	processCardToRunAssignment(card_id, run_id);
}

bool CardReaderPlugin::processCardToRunAssignment(int card_id, int run_id)
{
	bool is_relays = eventPlugin()->eventConfig()->isRelays();
	if(is_relays) {
		qf::core::sql::Query q;
		q.execThrow("SELECT relayId, leg, startTimeMs FROM runs WHERE id=" + QString::number(run_id));
		if(!q.next()) {
			qfError() << "Run not found, id:" << run_id;
			return false;
		}
		int relay_id = q.value(0).toInt();
		int leg = q.value(1).toInt();
		QVariant start_time = q.value(2);
		if(start_time.isNull() && leg > 1) {
			/// if strt time not set, take start time from previous leg
			q.execThrow("SELECT finishTimeMs FROM runs"
					 " WHERE relayId=" + QString::number(relay_id)
				   + " AND leg=" + QString::number(leg-1));
			if(q.next()) {
				int prev_finish_time = q.value(0).toInt();
				if(prev_finish_time > 0) {
					q.execThrow("UPDATE runs SET startTimeMs=" + QString::number(prev_finish_time)
						   + " WHERE relayId=" + QString::number(relay_id)
						   + " AND leg=" + QString::number(leg)
						   + " AND COALESCE(startTimeMs, 0)=0");
				}
			}
		}
		quickevent::core::si::CheckedCard checked_card = checkCard(card_id, run_id);
		updateCheckedCardValuesSql(checked_card);
		eventPlugin()->emitDbEvent(Event::EventPlugin::DBEVENT_CARD_PROCESSED_AND_ASSIGNED, checked_card, true);

		/// if next leg is finished and has not start time set, proces it too
		/// This covers cases when next leg is read-out before this one
		q.execThrow("SELECT id, startTimeMs, finishTimeMs FROM runs"
				 " WHERE relayId=" + QString::number(relay_id)
			   + " AND leg=" + QString::number(leg+1));
		if(q.next()) {
			start_time = q.value(1);
			int finish_time = q.value(2).toInt();
			if(finish_time > 0 && start_time.isNull()) {
				run_id = q.value(0).toInt();
				card_id = runsPlugin()->cardForRun(run_id);
				processCardToRunAssignment(card_id, run_id);
			}
		}
	}
	else {
		quickevent::core::si::CheckedCard checked_card = checkCard(card_id, run_id);
		updateCheckedCardValuesSql(checked_card);
		eventPlugin()->emitDbEvent(Event::EventPlugin::DBEVENT_CARD_PROCESSED_AND_ASSIGNED, checked_card, true);
	}
	return true;
}

int CardReaderPlugin::resolveAltCode(int maybe_alt_code, int stage_id)
{
	int resolved_code = 0;
	qfs::QueryBuilder qb;
	qb.select2("codes", "code")
			.from("codes")
			.joinRestricted("codes.id", "coursecodes.codeId", "codes.altCode=" QF_IARG(maybe_alt_code), qfs::QueryBuilder::INNER_JOIN)
			.joinRestricted("coursecodes.courseId", "classdefs.courseId", "classdefs.stageId=" QF_IARG(stage_id), qfs::QueryBuilder::INNER_JOIN);
	qfDebug() << qb.toString();
	qfs::Query q;
	q.exec(qb, qf::core::Exception::Throw);
	while (q.next()) {
		if(resolved_code) {
			qfError() << "duplicate alt code" << maybe_alt_code << "in stage:" << stage_id;
			return maybe_alt_code;
		}
		resolved_code = q.value(0).toInt();
	}
	if(resolved_code > 0)
		qfDebug() << "alt code:" << maybe_alt_code << "resolved to:" << resolved_code;
	return (resolved_code > 0)? resolved_code: maybe_alt_code;
}

}
