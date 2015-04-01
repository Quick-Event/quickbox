#ifndef CARDREADER_CARDCHECKER_H
#define CARDREADER_CARDCHECKER_H

#include <qf/core/utils.h>

#include <QObject>
#include <QVariant>

namespace CardReader {

class CardChecker : public QObject
{
	Q_OBJECT
	Q_PROPERTY(QString caption READ caption WRITE setCaption NOTIFY captionChanged)
public:
	//enum class CheckType : int {Classic = 0, BeerRace, Custom};
public:
	explicit CardChecker(QObject *parent = 0);

	QF_PROPERTY_IMPL(QString, c, C, aption)

	Q_INVOKABLE int toAMms(int time_msec);
	Q_INVOKABLE int toAM(int time_sec);
	Q_INVOKABLE int stageStartSec();
	Q_INVOKABLE int startTimeSec(int run_id);
	Q_INVOKABLE QVariantMap courseForRunId(int run_id);

	//Q_INVOKABLE QVariant checkCard(const QVariant &card, const QVariant &run_id);
};

}

#endif // CARDREADER_CARDCHECKER_H
