#ifndef CARDREADER_CARDREADERPLUGIN_H
#define CARDREADER_CARDREADERPLUGIN_H

#include "../cardreaderpluginglobal.h"

#include <qf/core/utils.h>
#include <qf/qmlwidgets/framework/plugin.h>

#include <QQmlListProperty>

namespace CardReader {

class CardChecker;
class ReadCard;
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

	QF_PROPERTY_IMPL2(int, c, C, urrentCardCheckerIndex, -1)

	const QList<CardReader::CardChecker*>& cardCheckers() {return m_cardCheckers;}
	CardReader::CardChecker* currentCardChecker();

	int currentStageId();
	int findRunId(int si_id);
	CheckedCard checkCard(int card_id);
	CheckedCard checkCard(const CardReader::ReadCard &read_card);
	int saveCardToSql(const CardReader::ReadCard &read_card);
	void updateRunLapsSql(const CardReader::CheckedCard &checked_card);
private:
	void onInstalled();
	QQmlListProperty<CardReader::CardChecker> cardCheckersListProperty();
private:
	QList<CardReader::CardChecker*> m_cardCheckers;
};

}

#endif
