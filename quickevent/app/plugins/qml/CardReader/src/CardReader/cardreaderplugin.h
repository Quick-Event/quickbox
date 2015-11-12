#ifndef CARDREADER_CARDREADERPLUGIN_H
#define CARDREADER_CARDREADERPLUGIN_H

#include "../cardreaderpluginglobal.h"

#include <qf/core/utils.h>
#include <qf/qmlwidgets/framework/plugin.h>

#include <QQmlListProperty>

class SIMessageTransmitRecord;

namespace CardReader {

class CardChecker;
class ReadCard;
class PunchRecord;
class CheckedCard;

class CARDREADERPLUGIN_DECL_EXPORT CardReaderPlugin : public qf::qmlwidgets::framework::Plugin
{
	Q_OBJECT
	Q_PROPERTY(QQmlListProperty<CardReader::CardChecker> cardCheckers READ cardCheckersListProperty)
	Q_PROPERTY(int currentCardCheckerIndex READ currentCardCheckerIndex WRITE setCurrentCardCheckerIndex NOTIFY currentCardCheckerIndexChanged)
private:
	typedef qf::qmlwidgets::framework::Plugin Super;
public:
	CardReaderPlugin(QObject *parent = nullptr);

	static const char* DBEVENTDOMAIN_CARDREADER_CARDREAD;
	static const char* DBEVENTDOMAIN_CARDREADER_PUNCHRECORD;
	static const QLatin1String SETTINGS_PREFIX;
	static const int FINISH_PUNCH_CODE;
	static const int FINISH_PUNCH_POS;

	QF_PROPERTY_IMPL2(int, c, C, urrentCardCheckerIndex, -1)

	const QList<CardReader::CardChecker*>& cardCheckers() {return m_cardCheckers;}
	CardReader::CardChecker* currentCardChecker();

	Q_INVOKABLE QString settingsPrefix();

	int currentStageId();
	int findRunId(int si_id);
	CheckedCard checkCard(int card_id, int run_id = 0);
	CheckedCard checkCard(const ReadCard &read_card);
	int saveCardToSql(const ReadCard &read_card);
	int savePunchRecordToSql(const PunchRecord &punch_record);
	//ReadCard loadCardFromSql(int card_id);
	bool updateCheckedCardValuesSql(const CheckedCard &checked_card);
	bool saveCardAssignedRunnerIdSql(int card_id, int run_id);

	Q_INVOKABLE bool reloadTimesFromCard(int card_id, int run_id = 0);
private:
	void onInstalled();
	QQmlListProperty<CardChecker> cardCheckersListProperty();
private:
	QList<CardChecker*> m_cardCheckers;
};

}

#endif
