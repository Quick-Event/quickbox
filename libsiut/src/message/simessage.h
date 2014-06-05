
//
// Author: Frantisek Vacek <fanda.vacek@volny.cz>, (C) 2011
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef SIMESSAGE_H
#define SIMESSAGE_H

#include "simessagedata.h"

#include <siut/siutglobal.h>

#include <QVariantMap>
#include <QCoreApplication>

class SIUT_DECL_EXPORT SIMessageBase
{
protected:
	SIMessageData f_data;
public:
	const SIMessageData &data() const {return f_data;}
	QString dump() const;
	//QVariant toVariant() const {return QVariant();}
	//int headerLength() const {return data().headerLength();}
	SIMessageData::Command command() const {return data().command();}
public:
	SIMessageBase() {}
	SIMessageBase(const SIMessageData &data) {f_data = data;}
};
//Q_DECLARE_METATYPE(SIMessageData);

class SIUT_DECL_EXPORT SIMessageCardReadOut : public SIMessageBase
{
	Q_DECLARE_TR_FUNCTIONS(SIMessageCardReadOut);
public:
	enum CardDataLayoutType {DataLayoutUnknown, DataLayout5, DataLayout6, DataLayout8, DataLayout9, DataLayout10};
	enum CardType {CardTypeUnknown, CardType5, CardType6, CardType8, CardType9, CardTypeP, CardTypeT, CardTypeSIAC, CardType10, CardType11};
protected:
	//void setCardDataLayoutType(CardDataLayoutType t) {f_data.dataRef()["cardDataLayoutType"] = t;}
	CardDataLayoutType cardDataLayoutType() const;
	int rawCardType() const;
	CardType cardType() const;
public:
	class SIUT_DECL_EXPORT Punch
	{
	private:
		class SharedDummyHelper {};
		class Data : public QSharedData
		{
		public:
			int flags;
			int code;
			int time;
			int timeMSec;
			Data() : flags(0), code(0), time(0), timeMSec(0) {}
			Data(const QByteArray &ba, int offset, int record_type);
		};
		QSharedDataPointer<Data> d;

		Punch(SharedDummyHelper); /// null row constructor
		static const Punch& sharedNull();
	public:
		int code() const;
		int time() const;
		int timeMSec() const;
		//bool is24HoursTimeFormat() const {return d->is24HoursTimeFormat;}
		/// 0-sunday
		int dayOfWeek() const;
		/// 4 week counter relative
		int weekCnt() const;

		Punch() : d() {}
		Punch(const QByteArray &ba, int offset, int record_type);// : d(ba, offset, record_type) {}
	};
	typedef QList<Punch>PunchList;
public:
	int stationCodeNumber() const;
	//int countryCode() const;
	//int clubCode() const;
	int cardNumber() const;
	//int startNumber() const;
	int startTime() const;
	int checkTime() const;
	int finishTime() const;
	PunchList punchList() const;
	QString dump() const;
	QVariant toVariant() const;
	static QString cardDataLayoutTypeToString(CardDataLayoutType card_layout_type);
	static QString cardTypeToString(CardType card_type);
public:
	SIMessageCardReadOut(const SIMessageData &data);
};
#if 0
class SIUT_DECL_EXPORT SIMessageCard5ReadOut : public SIMessageCardReadOutBase
{
	//Q_DECLARE_TR_FUNCTIONS(SIMessageCardReadOut);
public:
	virtual int stationCodeNumber() const;
	int countryCode() const;
	int clubCode() const;
	virtual int cardNumber() const;
	int startNumber() const;
	virtual int startTime() const;
	virtual int checkTime() const;
	virtual int finishTime() const;
	virtual PunchList punchList() const;
public:
	SIMessageCardReadOut(const SIMessageData &data) : SIMessageCardReadOutBase(data) {}
};
#endif
#endif // SIMESSAGE_H

