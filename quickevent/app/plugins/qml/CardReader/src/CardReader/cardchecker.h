#ifndef CARDREADER_CARDCHECKER_H
#define CARDREADER_CARDCHECKER_H

#include "checkedcard.h"

#include <qf/core/utils.h>

#include <QObject>
#include <QVariant>

namespace CardReader {

class ReadCard;

class CppCardChecker
{
public:
	virtual CheckedCard checkCard(const ReadCard &read_card) {Q_UNUSED(read_card) return CheckedCard();}
};

class CardChecker : public QObject
{
	Q_OBJECT
	Q_PROPERTY(QString caption READ caption WRITE setCaption NOTIFY captionChanged)
	Q_PROPERTY(int FINISH_PUNCH_CODE READ finishPunchCode)
public:
	//enum class CheckType : int {Classic = 0, BeerRace, Custom};
public:
	explicit CardChecker(QObject *parent = 0);

	QF_PROPERTY_IMPL(QString, c, C, aption)

	Q_INVOKABLE int fixTimeWrapAM(int time1_msec, int time2_msec);
	/// while time2 < time1 add 12 hours to time2 and return difference
	Q_INVOKABLE int msecIntervalAM(int time1_msec, int time2_msec);
	Q_INVOKABLE int toAMms(int time_msec);
	Q_INVOKABLE int toAM(int time_sec);
	Q_INVOKABLE int stageIdForRun(int run_id);
	Q_INVOKABLE int stageStartSec(int stage_id);
	Q_INVOKABLE int startTimeSec(int run_id);
	Q_INVOKABLE QVariantMap courseCodesForRunId(int run_id);

	static int finishPunchCode();

	//Q_INVOKABLE QVariant checkCard(const QVariant &card, const QVariant &run_id);
};

}

#endif // CARDREADER_CARDCHECKER_H
