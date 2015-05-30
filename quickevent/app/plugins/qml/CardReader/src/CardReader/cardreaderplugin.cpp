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

#include <QJSValue>
#include <QMetaObject>
#include <QSqlRecord>

namespace qff = qf::qmlwidgets::framework;

using namespace CardReader;

const char* CardReaderPlugin::DBEVENTDOMAIN_CARDREADER_CARDREAD = "CardReader.cardRead";

CardReaderPlugin::CardReaderPlugin(QObject *parent)
	: Super(parent)
{
	connect(this, &CardReaderPlugin::installed, this, &CardReaderPlugin::onInstalled);
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

CheckedCard CardReaderPlugin::checkCard(int card_id)
{
	qf::core::sql::Query q;
	if(q.exec("SELECT * FROM cards WHERE id=" QF_IARG(card_id))) {
		if(q.next()) {
			ReadCard rc(q.record());
			return checkCard(rc);
		}
	}
	qfWarning() << "Cannot find card record for id:" << card_id;
	return CheckedCard();
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

CardReader::CheckedCard CardReaderPlugin::checkCard(const ReadCard &read_card)
{
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

void CardReaderPlugin::updateRunLapsSql(const CardReader::CheckedCard &checked_card)
{
	qf::core::sql::Query q;
	int run_id = checked_card.runId();
	if(run_id <= 0)
		return;
	try {
		{
			q.exec("DELETE FROM runlaps WHERE runId=" QF_IARG(run_id), qf::core::Exception::Throw);
			q.prepare(QStringLiteral("INSERT INTO runlaps (runId, position, code, stpTimeMs, lapTimeMs) VALUES (:runId, :position, :code, :stpTimeMs, :lapTimeMs)"), qf::core::Exception::Throw);
			//q.prepare(QStringLiteral("INSERT INTO runlaps (runId, position, stpTimeMs, lapTimeMs) VALUES (:runId, :position, :stpTimeMs, :lapTimeMs)"), qf::core::Exception::Throw);
			auto punch_list = checked_card.punches();
			if(punch_list.count()) {
				{
					CardReader::CheckedPunch last_punch(punch_list.last().toMap());
					CardReader::CheckedPunch finish_punch;
					finish_punch.setPosition(punch_list.count() + 1);
					finish_punch.setCode(999);
					finish_punch.setStpTimeMs(checked_card.timeMs());
					if(last_punch.position() > 0) {
						finish_punch.setLapTimeMs(finish_punch.stpTimeMs() - last_punch.stpTimeMs());
					}
					punch_list << finish_punch;
				}
				for(auto v : punch_list) {
					CardReader::CheckedPunch cp(v.toMap());
					if(cp.position() > 0) {
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
			q.prepare("UPDATE runs SET timeMs=:timeMs, cardError=:cardError, status=:status WHERE id=" QF_IARG(run_id), qf::core::Exception::Throw);
			q.bindValue(QStringLiteral(":timeMs"), checked_card.timeMs());
			q.bindValue(QStringLiteral(":cardError"), !checked_card.isOk());
			q.bindValue(QStringLiteral(":status"), QStringLiteral("FINISH"));
			q.exec(qf::core::Exception::Throw);
		}
	}
	catch (const qf::core::Exception &e) {
		qfError() << trUtf8("Save card runlaps ERROR: %1").arg(q.lastErrorText());
	}
}

