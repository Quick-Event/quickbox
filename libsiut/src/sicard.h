#ifndef SIUT_SICARD_H
#define SIUT_SICARD_H

#include "sipunch.h"

#include <qf/core/utils.h>

#include <QSharedDataPointer>
#include <QVariantMap>
#include <QCoreApplication>

namespace siut {

class SIUT_DECL_EXPORT SICard
{
	Q_DECLARE_TR_FUNCTIONS(SICard)
public:
	using PunchList = QList<SIPunch>;
	static constexpr int INVALID_SI_TIME = 0xEEEE;
private:
	class SharedDummyHelper {};
	struct Data : public QSharedData
	{
		int stationNumber = 0;
		//int countryCode() const;
		//int clubCode() const;
		int cardNumber = 0;
		//int startNumber() const;
		int startTime = 0;
		int checkTime = 0;
		int finishTime = 0;
		SICard::PunchList punches;
	};
	QSharedDataPointer<Data> d;

	SICard(SharedDummyHelper); /// null
	static const SICard& sharedNull();
public:
	SICard();
	SICard(int card_number);

	bool isNull() const {return d == sharedNull().d;}

	QF_SHARED_CLASS_FIELD_RW(int, s, setS, tationNumber)
	QF_SHARED_CLASS_FIELD_RW(int, c, setC, ardNumber)
	QF_SHARED_CLASS_FIELD_RW(int, s, setS, tartTime)
	QF_SHARED_CLASS_FIELD_RW(int, c, setC, heckTime)
	QF_SHARED_CLASS_FIELD_RW(int, f, setF, inishTime)
	QF_SHARED_CLASS_FIELD_RW(SICard::PunchList, p, setP, unches)

	QString toString() const;
	QVariantMap toVariantMap() const;
	//static QString cardDataLayoutTypeToString(CardDataLayoutType card_layout_type);
	//static QString cardTypeToString(CardType card_type);
	static bool isTimeValid(int time);
	static int toAMms(int time_msec);
	static int toAM(int time_sec);
};

}

Q_DECLARE_METATYPE(siut::SICard)

#endif // SICARD_H
