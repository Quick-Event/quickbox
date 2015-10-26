#include "cardreaderplugin.h"
#include "readcard.h"
#include "checkedcard.h"
#include "cardchecker.h"
#include "../cardreaderpartwidget.h"

#include <Event/eventplugin.h>

#include <qf/qmlwidgets/framework/mainwindow.h>

#include <qf/core/log.h>
#include <qf/core/exception.h>
#include <qf/core/sql/query.h>
#include <qf/core/sql/connection.h>
#include <qf/core/sql/transaction.h>

#include <QJSValue>
#include <QMetaObject>
#include <QSqlRecord>

//#define QF_TIMESCOPE_ENABLED
#include <qf/core/utils/timescope.h>

namespace qff = qf::qmlwidgets::framework;

using namespace CardReader;

const char* CardReaderPlugin::DBEVENTDOMAIN_CARDREADER_CARDREAD = "CardReader.cardRead";
const char* CardReaderPlugin::DBEVENTDOMAIN_CARDREADER_PUNCHRECORD = "CardReader.punchRecord";
const QLatin1String CardReaderPlugin::SETTINGS_PREFIX("plugins/CardReader");
const int CardReaderPlugin::FINISH_PUNCH_CODE = 999;
const int CardReaderPlugin::FINISH_PUNCH_POS = CardReaderPlugin::FINISH_PUNCH_CODE;

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
	q.exec("SELECT id FROM runs WHERE stageId=" QF_IARG(stage_no) " AND siId=" QF_IARG(si_id), qf::core::Exception::Throw);
	int ret = 0;
	if(q.next()) {
		ret = q.value(0).toInt();
	}
	return ret;
}

CheckedCard CardReaderPlugin::checkCard(int card_id, int run_id)
{
	qfLogFuncFrame() << "run id:" << run_id << "card id:" << card_id;
	qf::core::sql::Query q;
	if(q.exec("SELECT * FROM cards WHERE id=" QF_IARG(card_id))) {
		if(q.next()) {
			ReadCard rc(q.record());
			if(run_id > 0)
				rc.setRunId(run_id);
			return checkCard(rc);
		}
	}
	qfWarning() << "Cannot find card record for id:" << card_id;
	return CheckedCard();
}

CardReader::CheckedCard CardReaderPlugin::checkCard(const ReadCard &read_card)
{
	qfLogFuncFrame();
	QF_TIME_SCOPE("checkCard()");
	//updateRunLapsSql(card, run_id);
	CardReader::CardChecker *chk = currentCardChecker();
	QF_ASSERT(chk != nullptr, "CardChecker is NULL", return CardReader::CheckedCard());
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

int CardReaderPlugin::saveCardToSql(const CardReader::ReadCard &read_card)
{
	int ret = 0;
	QStringList punches;
	for(auto v : read_card.punches()) {
		ReadPunch p(v.toMap());
		punches << p.toJsonArrayString();
	}
	qf::core::sql::Query q;
	q.prepare(QStringLiteral("INSERT INTO cards (stationNumber, siId, checkTime, startTime, finishTime, punches, runId, stageId)"
							 " VALUES (:stationNumber, :siId, :checkTime, :startTime, :finishTime, :punches, :runId, :stageId)")
			  , qf::core::Exception::Throw);
	q.bindValue(QStringLiteral(":stationNumber"), read_card.stationCodeNumber());
	q.bindValue(QStringLiteral(":siId"), read_card.cardNumber());
	q.bindValue(QStringLiteral(":checkTime"), read_card.checkTime());
	q.bindValue(QStringLiteral(":startTime"), read_card.startTime());
	q.bindValue(QStringLiteral(":finishTime"), read_card.finishTime());
	q.bindValue(QStringLiteral(":punches"), '[' + punches.join(", ") + ']');
	q.bindValue(QStringLiteral(":runId"), read_card.runId());
	q.bindValue(QStringLiteral(":stageId"), currentStageId());
	if(q.exec()) {
		ret = q.lastInsertId().toInt();
	}
	else {
		qfError() << trUtf8("Save card ERROR: %1").arg(q.lastErrorText());
	}
	return ret;
}

int CardReaderPlugin::savePunchRecordToSql(const PunchRecord &punch_record)
{
	int ret = 0;
	qf::core::sql::Query q;
	q.prepare(QStringLiteral("INSERT INTO punches (siId, code, punchTime, punchMs, runId, stageId)"
							 " VALUES (:siId, :code, :punchTime, :punchMs, :runId, :stageId)")
							, qf::core::Exception::Throw);
	q.bindValue(QStringLiteral(":siId"), punch_record.cardNumber());
	q.bindValue(QStringLiteral(":code"), punch_record.code());
	q.bindValue(QStringLiteral(":punchTime"), punch_record.time());
	q.bindValue(QStringLiteral(":punchMs"), punch_record.msec());
	q.bindValue(QStringLiteral(":runId"), punch_record.runId());
	q.bindValue(QStringLiteral(":stageId"), currentStageId());
	if(q.exec()) {
		ret = q.lastInsertId().toInt();
	}
	else {
		qfError() << trUtf8("Save punch record ERROR: %1").arg(q.lastErrorText());
	}
	return ret;
}

bool CardReaderPlugin::updateCheckedCardValuesSql(const CardReader::CheckedCard &checked_card)
{
	QF_TIME_SCOPE("updateCheckedCardValuesSql()");
	int run_id = checked_card.runId();
	//int card_id = checked_card.cardId();
	if(run_id <= 0)
		return false;
	auto cc = qf::core::sql::Connection::forName();
	qf::core::sql::Query q(cc);
	try {
		qf::core::sql::Transaction transaction(cc);
		{
			{
				QF_TIME_SCOPE("delete from runlaps");
				q.exec("DELETE FROM runlaps WHERE runId=" QF_IARG(run_id), qf::core::Exception::Throw);
			}
			q.prepare(QStringLiteral("INSERT INTO runlaps (runId, position, code, stpTimeMs, lapTimeMs) VALUES (:runId, :position, :code, :stpTimeMs, :lapTimeMs)"), qf::core::Exception::Throw);
			auto punch_list = checked_card.punches();
			if(punch_list.count()) {
				{
					CardReader::CheckedPunch finish_punch;
					finish_punch.setPosition(FINISH_PUNCH_POS); 
					finish_punch.setCode(FINISH_PUNCH_CODE);
					finish_punch.setStpTimeMs(checked_card.finishStpTimeMs());
					finish_punch.setLapTimeMs(checked_card.finishLapTimeMs());
					punch_list << finish_punch;
				}
				for(auto v : punch_list) {
					CardReader::CheckedPunch cp(v.toMap());
					qfInfo() << cp;
					if(cp.position() > 0 && cp.stpTimeMs() > 0 && cp.lapTimeMs() > 0) {
						q.bindValue(QStringLiteral(":runId"), run_id);
						q.bindValue(QStringLiteral(":code"), cp.code());
						q.bindValue(QStringLiteral(":position"), cp.position()); 
						q.bindValue(QStringLiteral(":stpTimeMs"), cp.stpTimeMs());
						q.bindValue(QStringLiteral(":lapTimeMs"), cp.lapTimeMs());
						q.exec(qf::core::Exception::Throw);
					}
				}
			}
		}
		{
			q.prepare("UPDATE runs SET timeMs=:timeMs, finishTimeMs=:finishTimeMs, misPunch=:misPunch, disqualified=:disqualified WHERE id=" QF_IARG(run_id), qf::core::Exception::Throw);
			q.bindValue(QStringLiteral(":timeMs"), checked_card.timeMs());
			q.bindValue(QStringLiteral(":finishTimeMs"), checked_card.finishTimeMs());
			q.bindValue(QStringLiteral(":misPunch"), !checked_card.isOk());
			q.bindValue(QStringLiteral(":disqualified"), !checked_card.isOk());
			q.exec(qf::core::Exception::Throw);
			if(q.numRowsAffected() != 1)
				return false;
		}
		transaction.commit();
		return true;
	}
	catch (const qf::core::Exception &e) {
		qfError() << trUtf8("Save card runlaps ERROR: %1").arg(q.lastErrorText());
	}
	return false;
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
	if(updateCheckedCardValuesSql(checked_card))
		if(saveCardAssignedRunnerIdSql(card_id, run_id))
			return true;
	return false;
}

