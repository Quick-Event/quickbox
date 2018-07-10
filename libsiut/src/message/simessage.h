
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
public:
	SIMessageBase() {}
	SIMessageBase(const SIMessageData &data) {m_data = data;}
public:
	const SIMessageData &data() const {return m_data;}
	QString dump() const;
	//QVariant toVariant() const {return QVariant();}
	//int headerLength() const {return data().headerLength();}
	SIMessageData::Command command() const {return data().command();}
protected:
	SIMessageData m_data;
};
//Q_DECLARE_METATYPE(SIMessageData);

class SIUT_DECL_EXPORT SIMessageCardReadOut : public SIMessageBase
{
	Q_DECLARE_TR_FUNCTIONS(SIMessageCardReadOut)
private:
	typedef SIMessageBase Super;
public:
	SIMessageCardReadOut(const SIMessageData &data);
public:
	enum CardDataLayoutType {DataLayoutUnknown, DataLayout5, DataLayout6, DataLayout8, DataLayout9, DataLayoutP, DataLayout10};
	enum CardType {CardTypeUnknown, CardType5, CardType6, CardType8, CardType9, CardTypeP, CardTypeT, CardTypeSIAC, CardType10, CardType11};
protected:
	enum PunchRecordType {PunchRecordDegraded, PunchRecordClasic, PunchRecordExtended};
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
			int msec;
			Data() : flags(0), code(0), time(0), msec(0) {}
			Data(const QByteArray &ba, int offset, int record_type);
		};
		QSharedDataPointer<Data> d;

		Punch(SharedDummyHelper); /// null row constructor
		static const Punch& sharedNull();
	public:
		int code() const {return d->code;}
		void setCode(int code) {d->code = code;}
		int time() const {return d->time;}
		void setTime(int tm) {d->time = tm;}
		int msec() const {return d->msec;}
		void setMsec(int ms) {d->msec = ms;}
		//bool is24HoursTimeFormat() const {return d->is24HoursTimeFormat;}
		/// 0-sunday
		int dayOfWeek() const;
		/// 4 week counter relative
		int weekCnt() const;

		QVariantMap toVariantMap() const;
		QString toString() const;

		Punch();
		Punch(const QByteArray &ba, int offset, PunchRecordType record_type);
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
	PunchList punches() const;
	QString dump() const;
	QVariantMap toVariantMap() const;
	static QString cardDataLayoutTypeToString(CardDataLayoutType card_layout_type);
	static QString cardTypeToString(CardType card_type);
	static bool isTimeValid(int time);
	static int toAM(int time_sec);
	static int toAMms(int time_msec);
};

class SIUT_DECL_EXPORT SIMessageTransmitPunch : public SIMessageBase
{
	Q_DECLARE_TR_FUNCTIONS(SIMessageTransmitPunch)
private:
	typedef SIMessageBase Super;
public:
	SIMessageTransmitPunch(const SIMessageData &data);

	int cardNumber() const {return m_cardNumber;}
	SIMessageCardReadOut::Punch punch() const {return m_punch;}
	QVariantMap toVariantMap() const;
private:
	int m_cardNumber = 0;
	SIMessageCardReadOut::Punch m_punch;
};

#endif // SIMESSAGE_H

