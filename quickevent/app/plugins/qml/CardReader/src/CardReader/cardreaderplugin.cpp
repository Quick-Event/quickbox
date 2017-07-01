#include "cardreaderplugin.h"
#include "readcard.h"
#include "checkedcard.h"
#include "cardchecker.h"
#include "../cardreaderpartwidget.h"

#include <Event/eventplugin.h>

#include <quickevent/og/timems.h>
#include <quickevent/si/punchrecord.h>

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

const QLatin1String CardReaderPlugin::SETTINGS_PREFIX("plugins/CardReader");
const int CardReaderPlugin::FINISH_PUNCH_POS = quickevent::si::PunchRecord::FINISH_PUNCH_CODE;

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

int CardReaderPlugin::findRunId(int si_id)
{
	int stage_no = currentStageId();
	qf::core::sql::Query q;
	q.exec("SELECT id FROM runs WHERE stageId=" QF_IARG(stage_no) " AND siId=" QF_IARG(si_id) " AND isRunning", qf::core::Exception::Throw);
	int ret = 0;
	if(q.next()) {
		ret = q.value(0).toInt();
	}
	return ret;
}

ReadCard CardReaderPlugin::readCard(int card_id)
{
	qfLogFuncFrame() << "card id:" << card_id;
	qf::core::sql::Query q;
	if(q.exec("SELECT * FROM cards WHERE id=" QF_IARG(card_id))) {
		if(q.next()) {
			ReadCard rc(q.record());
			return rc;
		}
	}
	qfWarning() << "Cannot find card record for id:" << card_id;
	return ReadCard();
}

CheckedCard CardReaderPlugin::checkCard(int card_id, int run_id)
{
	qfLogFuncFrame() << "run id:" << run_id << "card id:" << card_id;
	ReadCard rc = readCard(card_id);
	if(!rc.isEmpty()) {
		if(run_id > 0)
			rc.setRunId(run_id);
		return checkCard(rc);
	}
	return CheckedCard();
}

CardReader::CheckedCard CardReaderPlugin::checkCard(const ReadCard &read_card)
{
	qfLogFuncFrame();
	QF_TIME_SCOPE("checkCard()");
	//updateRunLapsSql(card, run_id);
	CardReader::CardChecker *chk = currentCardChecker();
	QF_ASSERT(chk != nullptr, "CardChecker is NULL", return CardReader::CheckedCard());
	CardReader::CppCardChecker *cpp_chk = dynamic_cast<CardReader::CppCardChecker*>(chk);
	if(cpp_chk) {
		CheckedCard cc = cpp_chk->checkCard(read_card);
		return cc;
	}
	else {
		QVariant ret_val;
		QMetaObject::invokeMethod(chk, "checkCard", Qt::DirectConnection,
								  Q_RETURN_ARG(QVariant, ret_val),
								  Q_ARG(QVariant, read_card));
		QJSValue jsv = ret_val.value<QJSValue>();
		QVariant v = jsv.toVariant();
		QVariantMap m = v.toMap();
		CardReader::CheckedCard cc(m);
		cc.setCardNumber(read_card.cardNumber());
		//cc.setCardId(read_card.cardId());
		return cc;
	}
}

int CardReaderPlugin::saveCardToSql(const CardReader::ReadCard &read_card)
{
	int ret = 0;
	QStringList punches;
	for(auto v : read_card.punches()) {
		ReadPunch p(v.toMap());
		punches << p.toJsonArrayString();
	}
	qf::core::sql::Query q;
	q.prepare(QStringLiteral("INSERT INTO cards (stationNumber, siId, checkTime, startTime, finishTime, punches, runId, stageId, readerConnectionId)"
							 " VALUES (:stationNumber, :siId, :checkTime, :startTime, :finishTime, :punches, :runId, :stageId, :readerConnectionId)")
			  , qf::core::Exception::Throw);
	q.bindValue(QStringLiteral(":stationNumber"), read_card.stationCodeNumber());
	q.bindValue(QStringLiteral(":siId"), read_card.cardNumber());
	q.bindValue(QStringLiteral(":checkTime"), read_card.checkTime());
	q.bindValue(QStringLiteral(":startTime"), read_card.startTime());
	q.bindValue(QStringLiteral(":finishTime"), read_card.finishTime());
	q.bindValue(QStringLiteral(":punches"), '[' + punches.join(", ") + ']');
	q.bindValue(QStringLiteral(":runId"), read_card.runId());
	q.bindValue(QStringLiteral(":stageId"), currentStageId());
	q.bindValue(QStringLiteral(":readerConnectionId"), qf::core::sql::Connection::defaultConnection().connectionId());
	if(q.exec()) {
		ret = q.lastInsertId().toInt();
	}
	else {
		qfError() << trUtf8("Save card ERROR: %1").arg(q.lastErrorText());
	}
	return ret;
}

int CardReaderPlugin::savePunchRecordToSql(const quickevent::si::PunchRecord &punch_record)
{
	//qfInfo() << "PUNCH:" << punch_record.toString();
	int ret = 0;
	quickevent::si::PunchRecord punch = punch_record;
	punch.setstageid(currentStageId());

	Event::EventPlugin *event_plugin = eventPlugin();
	int stage_start_msec = event_plugin->stageStartMsec(event_plugin->currentStageId());
	int time_msec = quickevent::og::TimeMs::msecIntervalAM(stage_start_msec, punch_record.time() * 1000 + punch_record.msec());
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
		qfError() << trUtf8("Save punch record ERROR: %1").arg(q.lastErrorText());
	}
	return ret;
}

bool CardReaderPlugin::updateCheckedCardValuesSqlSafe(const CheckedCard &checked_card)
{
	try {
		qf::core::sql::Transaction transaction;
		updateCheckedCardValuesSql(checked_card);
		transaction.commit();
		return true;
	}
	catch (const qf::core::Exception &e) {
		qfError() << trUtf8("Update runs & runlaps ERROR:") << e.message();
	}
	return false;
}

void CardReaderPlugin::updateCheckedCardValuesSql(const CardReader::CheckedCard &checked_card) throw(qf::core::Exception)
{
	QF_TIME_SCOPE("updateCheckedCardValuesSql()");
	int run_id = checked_card.runId();
	if(run_id <= 0)
		QF_EXCEPTION("Card doesn't contain runId information!");
	auto cc = qf::core::sql::Connection::forName();
	qf::core::sql::Query q(cc);
	{
		QF_TIME_SCOPE("DELETE FROM runlaps");
		q.exec("DELETE FROM runlaps WHERE runId=" QF_IARG(run_id), qf::core::Exception::Throw);
	}
	q.prepare(QStringLiteral("INSERT INTO runlaps (runId, position, code, stpTimeMs, lapTimeMs) VALUES (:runId, :position, :code, :stpTimeMs, :lapTimeMs)"), qf::core::Exception::Throw);
	auto punch_list = checked_card.punches();
	if(punch_list.count()) {
		int position = 0;
		for(auto v : punch_list) {
			position++;
			CardReader::CheckedPunch cp(v.toMap());
			//qfInfo() << run_id << position << cp;
			if(cp.stpTimeMs() > 0 && cp.lapTimeMs() > 0) {
				q.bindValue(QStringLiteral(":runId"), run_id);
				q.bindValue(QStringLiteral(":code"), cp.code());
				q.bindValue(QStringLiteral(":position"), position);
				q.bindValue(QStringLiteral(":stpTimeMs"), cp.stpTimeMs());
				q.bindValue(QStringLiteral(":lapTimeMs"), cp.lapTimeMs());
				q.exec(qf::core::Exception::Throw);
			}
		}
	}
	q.prepare("UPDATE runs SET timeMs=:timeMs, finishTimeMs=:finishTimeMs, misPunch=:misPunch, disqualified=:disqualified WHERE id=" QF_IARG(run_id), qf::core::Exception::Throw);
	q.bindValue(QStringLiteral(":timeMs"), checked_card.timeMs());
	q.bindValue(QStringLiteral(":finishTimeMs"), checked_card.finishTimeMs());
	q.bindValue(QStringLiteral(":misPunch"), checked_card.isMisPunch());
	q.bindValue(QStringLiteral(":disqualified"), !checked_card.isOk());
	q.exec(qf::core::Exception::Throw);
	if(q.numRowsAffected() != 1)
		QF_EXCEPTION("Update runs error!");
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
	if(!card_id)
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
	CardReader::CheckedCard checked_card = checkCard(card_id, run_id);
	//qfInfo() << Q_FUNC_INFO << checked_card.isMisPunch() << checked_card.isOk();
	if(updateCheckedCardValuesSqlSafe(checked_card))
		if(saveCardAssignedRunnerIdSql(card_id, run_id))
			return true;
	return false;
}

int CardReaderPlugin::resolveAltCode(int maybe_alt_code, int stage_id)
{
	qfs::QueryBuilder qb;
	qb.select2("codes", "code")
			.from("codes")
			.joinRestricted("codes.id", "coursecodes.codeId", "codes.altCode > 0", qfs::QueryBuilder::INNER_JOIN)
			.joinRestricted("coursecodes.courseId", "classdefs.courseId", "classdefs.stageId=" QF_IARG(stage_id), qfs::QueryBuilder::INNER_JOIN);
	qfDebug() << qb.toString();
	qfs::Query q;
	q.exec(qb, qf::core::Exception::Throw);
	int resolved_code = 0;
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
