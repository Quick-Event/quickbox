
//
// Author: Frantisek Vacek <fanda.vacek@volny.cz>, (C) 2011
//
// Copyright: See COPYING file that comes with this distribution
//

#include "simessage.h"

#include <qf/core/log.h>

#include <QStringList>

//=================================================
//             SIMessageBase
//=================================================
QString SIMessageBase::dump() const
{
	QString ret = f_data.dump();
	return ret;
}

//====================================================
//            SIMessageCardReadOut
//====================================================
SIMessageCardReadOut::Punch::Punch(SIMessageCardReadOut::Punch::SharedDummyHelper)
{
	d = new Data();
}

SIMessageCardReadOut::Punch::Punch()
{
	*this = sharedNull();
}

const SIMessageCardReadOut::Punch & SIMessageCardReadOut::Punch::sharedNull()
{
	static SIMessageCardReadOut::Punch n = SIMessageCardReadOut::Punch(SharedDummyHelper());
	return n;
}

SIMessageCardReadOut::Punch::Punch(const QByteArray& ba, int offset, int record_type)
{
	d = new Data();
	if(record_type == 2) {
		//d->is24HoursTimeFormat = false;
		//flags = 0;
		if(ba.length() > offset + 2) {
			d->code = (unsigned char)ba[offset];
			d->time = (((int)(unsigned char)ba[offset + 1]) << 8) + (unsigned char)ba[offset + 2];
		}
		else qfError() << "Incorrect punch data:" << SIMessageData::dumpData(ba) << "correct length is:" << offset + 3 << "offset:" << offset;
	}
	else if(record_type == 1) {
		//d->is24HoursTimeFormat = false;
		//flags = 0;
		if(ba.length() > offset) {
			d->code = (unsigned char)ba[offset];
		}
		else qfError() << "Incorrect punch data:" << SIMessageData::dumpData(ba) << "correct length is:" << offset + 0 << "offset:" << offset;
		d->time = 0xEEEE;
	}
	else if(record_type == 4) {
		//d->is24HoursTimeFormat = true;
		if(ba.length() > offset + 3) {
			d->flags = (unsigned char)ba[offset];
			/** PTD flags
			 * bit 0 - am/pm
			 * bit 3...1 - day of week, 000 = Sunday, 110 = Saturday
			 * bit 5...4 - week counter 0…3, relative
			 * bit 7...6 - control station code number high
			 *
			 */
			d->code = (unsigned char)ba[offset + 1];
			if(d->flags & (1<<7)) d->code = 0;
			else d->code += 256 * ((d->flags & (3 << 6)) >> 6);
			/** code
			 * CN - control station code number, 0...255 or subsecond value1
			 * subsecond value only for “start” and “finish” possible
			 * new from sw5.49: bit7=1 in PTD-byte indicates a subsecond value in CN byte (use always code numbers <256 for start/finish)
			 */
			d->time = (((int)(unsigned char)ba[offset + 2]) << 8) + (unsigned char)ba[offset + 3];
			if(d->flags & (1 << 0)) d->time += 12*60*60;
			//qfInfo() << ba.mid(offset, 4).toHex();
			//qfInfo() << "code:" << d->code;
		}
		else qfError() << "Incorrect punch data:" << SIMessageData::dumpData(ba) << "correct length is:" << offset + 4 << "offset:" << offset;
	}
	else {
		qfError() << "Incorrect punch record type:" << record_type;
	}
}

int SIMessageCardReadOut::Punch::code() const
{
	int ret = d->code;
	return ret;
}

int SIMessageCardReadOut::Punch::time() const
{
	int ret = d->time;
	//if(is24HoursTimeFormat() && d.flags & (1<<0)) ret += 12*60*60;
	return ret;
}

int SIMessageCardReadOut::Punch::timeMSec() const
{
	int ret = d->timeMSec;
	/*
	if(d.flags & (1<<7)) {
		ret = d.code; /// 1/256 sec
		ret = 1000 * ret / 256;
	}
	*/
	return ret;
}

int SIMessageCardReadOut::Punch::dayOfWeek() const
{
	int ret = (d->flags & (7 << 1)) >> 1;
	return ret;
}

int SIMessageCardReadOut::Punch::weekCnt() const
{
	int ret = (d->flags & (3 << 4)) >> 4;
	return ret;
}

QString SIMessageCardReadOut::Punch::toJsonArrayString() const
{
	QString ret = QStringLiteral("[%1, %2, %3, %4]");
	ret = ret.arg(code()).arg(time() * 1000 + timeMSec()).arg(dayOfWeek()).arg(weekCnt());
	return ret;
}

static int to_AM(int secs)
{
	while(secs > 12*60*60) secs -= 12*60*60;
	while(secs < 0) secs += 12*60*60;
	return secs;
}

static QString time_str(int _time)
{
	QString ret = "%1:%2.%3";
	if(_time == 0xEEEE) ret = "----";
	else {
		int time = to_AM(_time);
		ret = ret.arg(time / (60*60)).arg(QString::number((time / 60) % 60), 2, '0').arg(QString::number(time % 60), 2, '0');
	}
	return ret;
}

static QString ob_time_str(int _time)
{
	QString ret = "%1.%2";
	if(_time == 0xEEEE) ret = "----";
	else {
		int time = to_AM(_time);
		ret = ret.arg(time / 60).arg(QString::number(time % 60), 2, '0');
	}
	return ret;
}

SIMessageCardReadOut::SIMessageCardReadOut(const SIMessageData& _data)
	: SIMessageBase(_data)
{
}

QString SIMessageCardReadOut::cardTypeToString(SIMessageCardReadOut::CardType card_type)
{
	QString ret;
	switch(card_type) {
		case CardType5: ret = "CardType5"; break;
		case CardType6: ret = "CardType6"; break;
		case CardType8: ret = "CardType8"; break;
		case CardType9: ret = "CardType9"; break;
		case CardTypeP: ret = "CardTypeP"; break;
		case CardTypeT: ret = "CardTypeT"; break;
		case CardTypeSIAC: ret = "CardTypeSIAC"; break;
		case CardType10: ret = "CardType10"; break;
		case CardType11: ret = "CardType11"; break;
		default: ret = "CardTypeUnknown"; break;
	}
	return ret;
}

QString SIMessageCardReadOut::cardDataLayoutTypeToString(SIMessageCardReadOut::CardDataLayoutType card_layout_type)
{
	QString ret;
	switch(card_layout_type) {
		case DataLayout5: ret = "DataLayout5"; break;
		case DataLayout6: ret = "DataLayout6"; break;
		case DataLayout8: ret = "DataLayout8"; break;
		case DataLayout9: ret = "DataLayout9"; break;
		case DataLayout10: ret = "DataLayout10"; break;
		default: ret = "DataLayoutUnknown"; break;
	}
	return ret;
}

SIMessageCardReadOut::CardDataLayoutType SIMessageCardReadOut::cardDataLayoutType() const
{
	CardDataLayoutType ret = DataLayoutUnknown;
	switch(cardType()) {
	case CardType5:
		ret = DataLayout5;
		break;
	case CardType6:
		ret = DataLayout6;
		break;
	case CardType8:
		ret = DataLayout8;
		break;
	case CardType9:
	case CardTypeP:
	case CardTypeT:
		ret = DataLayout9;
		break;
	case CardTypeSIAC:
	case CardType10:
	case CardType11:
		ret = DataLayout10;
		break;
	default:
		qfError() << "Can't assign cardDataLayout for card type:" << rawCardType() << cardTypeToString(cardType());
		ret = DataLayoutUnknown;
		break;
	}
	return ret;
}

int SIMessageCardReadOut::rawCardType() const
{
	int ret = -1;
	switch(command()) {
		case SIMessageData::CmdGetSICard5:
		case SIMessageData::CmdGetSICard5Ext:
		case SIMessageData::CmdGetSICard6:
		case SIMessageData::CmdGetSICard6Ext:
			break;
		case SIMessageData::CmdGetSICard8Ext:
		{
			QByteArray raw_data = data().blockData(0);
			ret = (int)(((unsigned char)raw_data[6 * 4]) & 0x0F);
			//qfInfo() << "raw card type:" << ret;
			break;
		}
		default:
			ret = -1;
			break;
	}
	return ret;
}

SIMessageCardReadOut::CardType SIMessageCardReadOut::cardType() const
{
	CardType ret = CardTypeUnknown;
	switch(command()) {
		case SIMessageData::CmdGetSICard5:
		case SIMessageData::CmdGetSICard5Ext:
			ret = CardType5;
			break;
		case SIMessageData::CmdGetSICard6:
		case SIMessageData::CmdGetSICard6Ext:
			ret = CardType6;
			break;
		case SIMessageData::CmdGetSICard8Ext:
		{
			QByteArray raw_data = data().blockData(0);
			int n = rawCardType();
			//qfInfo() << "raw card type:" << n;
			switch(n) {
				case 1: ret = CardType9; break;
				case 2: ret = CardType8; break;
				case 4: ret = CardTypeP; break;
				case 6: ret = CardTypeT; break;
				case 15: ret = CardTypeSIAC; break;
				default: ret = CardTypeUnknown; break;
			}
			break;
		}
		default:
			qfError() << "Can't assign cardType for command:" << command() << SIMessageData::commandName(command());
			ret = CardTypeUnknown;
			break;
	}
	return ret;
}

QString SIMessageCardReadOut::dump() const
{
	QStringList sl;
	sl << data().dump();
	sl << tr("cardType: %1 (raw type: %2)").arg(cardTypeToString(cardType())).arg(rawCardType());
	sl << tr("stationCodeNumber: %1").arg(stationCodeNumber());
	sl << tr("cardNumber: %1").arg(cardNumber());
	//sl << tr("startNumber: %1").arg(startNumber());
	//sl << tr("countryCode: %1").arg(countryCode());
	//sl << tr("clubCode: %1").arg(clubCode());
	int start = startTime();
	if(start == 0xEEEE) start = checkTime();
	sl << tr("check: %1").arg(time_str(checkTime()));
	sl << tr("start: %1").arg(time_str(startTime()));
	sl << tr("finish: %1 (%2)").arg(time_str(finishTime())).arg(ob_time_str(finishTime() - start));
	int i = 0;
	foreach(const Punch &p, punchList()) {
		sl << QString::number(++i) + ".\t" + QString::number(p.code()) + "\t" + ob_time_str(p.time()) + "\t" + ob_time_str(p.time() - start);
	}
	return sl.join("\n");
}

QVariant SIMessageCardReadOut::toVariant() const
{
	QVariantMap ret;// = SIMessageBase::toVariant().toMap();
	ret["stationCodeNumber"] = stationCodeNumber();
	ret["cardNumber"] = cardNumber();
	ret["cardNumberFull"] = (cardNumber() < 100000)? 100000 + cardNumber(): cardNumber();
	//ret["startNumber"] = startNumber();
	//ret["countryCode"] = countryCode();
	//ret["clubCode"] = clubCode();
	ret["checkTime"] = checkTime();
	ret["startTime"] = startTime();
	ret["finishTime"] = finishTime();
	QVariantList punch_list;
	foreach(const Punch &p, punchList()) {
		QVariantMap m;
		m["code"] = p.code();
		m["time"] = p.time();
		punch_list << m;
	}
	ret["punchList"] = punch_list;
	return ret;
}

int SIMessageCardReadOut::stationCodeNumber() const
{
	QByteArray raw_data = data().blockData(0);
	int ret = (((int)(unsigned char)raw_data[2]) << 8) + (unsigned char)raw_data[3];
	return ret;
}
/*
int SIMessageCardReadOut::countryCode() const
{
	QByteArray raw_data = data().rawData();
	int base = CARD5EXT_RECORD_BASE + 1;
	int ret = (unsigned char)raw_data[base];
	return ret;
}

int SIMessageCardReadOut::clubCode() const
{
	QByteArray raw_data = data().rawData();
	int base = CARD5EXT_RECORD_BASE + 2;
	int ret = (((int)(unsigned char)raw_data[base]) << 8) + (unsigned char)raw_data[base + 1];
	return ret;
}
*/
int SIMessageCardReadOut::cardNumber() const
{
	int ret = 0;
	QByteArray raw_data = data().blockData(0);
	CardDataLayoutType card_data_layout_type = cardDataLayoutType();
	if(card_data_layout_type == DataLayout5) {
		int base = 4;
		ret = (((int)(unsigned char)raw_data[base]) << 8) + (unsigned char)raw_data[base + 1];
		int cs = (unsigned char)raw_data[base + 2];
		if(cs > 1)
			ret += 100000 * cs;
		//qfInfo() << SIMessageData::dumpData(raw_data);
		//qfInfo() << "n:" << ret << "cs:" << cs;
		//ret += (cs << 16);
	}
	else if(card_data_layout_type == DataLayout6) {
		int base = 2*4;
		//QByteArray ba = raw_data.mid(CARD8_RECORD_BASE + 2*4, 16);
		//qfInfo() << ba.toHex();
		ret = (((int)(unsigned char)raw_data[base+3]) << 16) + (((int)(unsigned char)raw_data[base+4]) << 8) + (unsigned char)raw_data[base+5];
	}
	else if(card_data_layout_type >= DataLayout8 && card_data_layout_type <= DataLayout10) {
		int base = 24;
		//QByteArray ba = raw_data.mid(CARD8_RECORD_BASE + 24, 16);
		//qfInfo() << ba.toHex();
		ret = (((int)(unsigned char)raw_data[base+1]) << 16) + (((int)(unsigned char)raw_data[base+2]) << 8) + (unsigned char)raw_data[base+3];
	}
	if(ret < 20000)
		ret = ret % 10000;
	return ret;
}
/*
int SIMessageCardReadOut::startNumber() const
{
	QByteArray raw_data = data().rawData();
	int base = CARD5EXT_RECORD_BASE + 0x11;
	int ret = (((int)(unsigned char)raw_data[base]) << 8) + (unsigned char)raw_data[base + 1];
	int cs = (unsigned char)raw_data[base + 2];
	if(cs == 1) cs = 0;
	ret += (cs << 16);
	return ret;
}
*/
int SIMessageCardReadOut::checkTime() const
{
	int ret = 0;
	QByteArray raw_data = data().blockData(0);
	CardDataLayoutType card_data_layout_type = cardDataLayoutType();
	if(card_data_layout_type == DataLayout5) {
		int base = 0x19;
		ret = (((int)(unsigned char)raw_data[base]) << 8) + (unsigned char)raw_data[base + 1];
	}
	else if(card_data_layout_type == DataLayout6) {
		int base = 7*4;
		ret = (((int)(unsigned char)raw_data[base + 2]) << 8) + (unsigned char)raw_data[base + 3];
	}
	else if(card_data_layout_type >= DataLayout8 && card_data_layout_type <= DataLayout10) {
		int base = 2*4;
		ret = (((int)(unsigned char)raw_data[base + 2]) << 8) + (unsigned char)raw_data[base + 3];
	}
	return ret;
}

int SIMessageCardReadOut::startTime() const
{
	int ret = 0;
	QByteArray raw_data = data().blockData(0);
	CardDataLayoutType card_data_layout_type = cardDataLayoutType();
	if(card_data_layout_type == DataLayout5) {
		int base = 0x13;
		ret = (((int)(unsigned char)raw_data[base]) << 8) + (unsigned char)raw_data[base + 1];
	}
	else if(card_data_layout_type == DataLayout6) {
		int base = 6*4;
		ret = (((int)(unsigned char)raw_data[base + 2]) << 8) + (unsigned char)raw_data[base + 3];
	}
	else if(card_data_layout_type >= DataLayout8 && card_data_layout_type <= DataLayout10) {
		int base = 3*4;
		ret = (((int)(unsigned char)raw_data[base + 2]) << 8) + (unsigned char)raw_data[base + 3];
	}
	return ret;
}

int SIMessageCardReadOut::finishTime() const
{
	int ret = 0;
	QByteArray raw_data = data().blockData(0);
	CardDataLayoutType card_data_layout_type = cardDataLayoutType();
	if(card_data_layout_type == DataLayout5) {
		int base = 0x15;
		ret = (((int)(unsigned char)raw_data[base]) << 8) + (unsigned char)raw_data[base + 1];
	}
	else if(card_data_layout_type == DataLayout6) {
		int base = 5*4;
		ret = (((int)(unsigned char)raw_data[base + 2]) << 8) + (unsigned char)raw_data[base + 3];
	}
	else if(card_data_layout_type >= DataLayout8 && card_data_layout_type <= DataLayout10) {
		int base = 4*4;
		ret = (((int)(unsigned char)raw_data[base + 2]) << 8) + (unsigned char)raw_data[base + 3];
	}
	return ret;
}

SIMessageCardReadOut::PunchList SIMessageCardReadOut::punchList() const
{
	qfLogFuncFrame();
	PunchList ret;
	CardDataLayoutType card_data_layout_type = cardDataLayoutType();
	if(card_data_layout_type == DataLayout5) {
		QByteArray raw_data = data().blockData(0);
		int punch_cnt = raw_data[0x17];
		punch_cnt--;
		int base = 0x20;
		for(int i=0; i<30 && i<punch_cnt; i++) {
			int offset = 3*i + i/5 + 1;
			//qfInfo() << i << "->" << QString::number(offset, 16);
			Punch p(raw_data, base + offset, 2);
			ret << p;
		}
		for(int i=30; i<36 && i<punch_cnt; i++) {
			int offset = 16*(i-30);
			Punch p(raw_data, base + offset, 1);
			ret << p;
		}
	}
	else if(card_data_layout_type == DataLayout6) {
		QByteArray raw_data = data().blockData(0);
		int punch_cnt = (unsigned char)raw_data[4*4 + 2];
		/// blocks 6,7,2,3,4,5
		/// each block has 32 punches
		/// each punch has 4 bytes
		int curr_block_no = -1;
		for(int i=0; i<punch_cnt; i++) {
			int block_no = i / 32;
			if(block_no < 2) block_no += 6;
			if(curr_block_no != block_no) {
				raw_data = data().blockData(block_no);
				curr_block_no = block_no;
			}
			int offset = (i % 32) * 4;
			//qfInfo() << raw_data.mid(offset, 16).toHex();
			Punch p(raw_data, offset, 4);
			ret << p;
		}
	}
	else if(card_data_layout_type == DataLayout8) {
		QByteArray raw_data = data().blockData(0);
		int punch_cnt = (unsigned char)raw_data[5*4 + 2];
		/// blocks 1
		/// block has up to 30 punches
		/// each punch has 4 bytes
		raw_data = data().blockData(1);
		int base = 2*4;
		for(int i=0; i<punch_cnt; i++) {
			int offset = 4*i;
			//if(i >= (32-14)) offset += dataOffsetInPacket(); /// dalsi packet
			qfInfo() << i << "->" << QString::number(offset, 16);
			Punch p(raw_data, base + offset, 4);
			ret << p;
		}
	}
	else if(card_data_layout_type == DataLayout9) {
		QByteArray raw_data = data().blockData(0);
		int punch_cnt = (unsigned char)raw_data[5*4 + 2];
		/// blocks 0, 1
		/// block 0 has 18 punches starting on page 14
		/// block 1 has 32 punches starting on page 0
		/// each punch has 4 bytes
		int base = 14*4;
		for(int i=0; i<punch_cnt; i++) {
			int offset = 0;
			if(i < 18) {
				raw_data = data().blockData(0);
				base = 14 * 4;
				offset = i * 4;
				//qfInfo() << i << "->" << "base page:" << (base / 4) << "offset page:" << (offset / 4);
			}
			else {
				raw_data = data().blockData(1);
				base = 0;
				offset = (i - 18) * 4;
				//qfInfo() << i << "->" << "base page:" << (base / 4) << "offset page:" << (offset / 4);
			}
			//qfInfo() << i << "->" << QString::number(offset, 16);
			Punch p(raw_data, base + offset, 4);
			ret << p;
		}
	}
	else if(card_data_layout_type == DataLayout10) {
		QByteArray raw_data = data().blockData(0);
		int punch_cnt = (unsigned char)raw_data[5*4 + 2];
		/// blocks 4,5,6,7
		/// each block has 32 punches
		/// each punch has 4 bytes
		for(int i=0; i<punch_cnt; i++) {
			int block_no = i / 32 + 4;
			raw_data = data().blockData(block_no);
			int offset = (i % 32) * 4;
			//qfInfo() << raw_data.mid(offset, 16).toHex();
			Punch p(raw_data, offset, 4);
			ret << p;
		}
	}
	return ret;
}

