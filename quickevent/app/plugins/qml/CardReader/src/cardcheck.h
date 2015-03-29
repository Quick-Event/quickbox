#ifndef CARDCHECK_H
#define CARDCHECK_H

#include <QObject>

class CardCheck : public QObject
{
	Q_OBJECT
public:
	enum class CheckType : int {Classic = 0, BeerRace, Custom};
public:
	explicit CardCheck(QObject *parent = 0);

	//Q_INVOKABLE CheckResult checkCard(const SIMessageCardReadOut &card, int run_id);
};

#endif // CARDCHECK_H
