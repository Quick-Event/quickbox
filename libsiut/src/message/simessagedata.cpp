
//
// Author: Frantisek Vacek <fanda.vacek@volny.cz>, (C) 2011
//
// Copyright: See COPYING file that comes with this distribution
//

#include "simessage.h"

#include <QStringList>

#include <qf/core/logcust.h>

namespace
{
	class SIMessageDataMetaTypeInit
	{
	public:
		SIMessageDataMetaTypeInit()
		{
			qRegisterMetaType<SIMessageData>("SIMessageData");
		}
	};

	SIMessageDataMetaTypeInit initializer;
}

//=================================================
//             SIMessageBase
//=================================================
SIMessageData::Command SIMessageData::command(const QByteArray &raw_data_with_header)
{
	Command ret = CmdInvalid;
	if(raw_data_with_header.length() > 0) {
		ret = (Command)(unsigned char)raw_data_with_header[0];
	}
	else {
		qfError() << "empty message";
		qfInfo() << qf::core::Log::stackTrace();
	}
	return ret;
}

const char* SIMessageData::commandName(SIMessageData::Command cmd)
{
	static QByteArray unknown_command_helper;
	const char *ret;
	switch(cmd) {
		case CmdSICard5DetectedExt: ret = "CmdSICard5DetectedExt"; break;
		case CmdSICard6DetectedExt: ret = "CmdSICard6DetectedExt"; break;
		case CmdSICard8AndHigherDetectedExt: ret = "CmdSICard8AndHigherDetectedExt"; break;
		case CmdSICardRemovedExt: ret = "CmdSICardRemovedExt"; break;
		case CmdSICardDetectedOrRemoved: ret = "CmdSICardDetectedOrRemoved"; break;
		case CmdGetSICard5: ret = "CmdGetSICard5"; break;
		case CmdGetSICard5Ext: ret = "CmdGetSICard5Ext"; break;
		case CmdGetSICard6: ret = "CmdGetSICard6"; break;
		case CmdGetSICard6Ext: ret = "CmdGetSICard6Ext"; break;
		case CmdGetSICard8Ext: ret = "CmdGetSICard8Ext"; break;
		case CmdGetPunch2: ret = "CmdGetPunch2"; break;
		case CmdTimeSend: ret = "CmdTimeSend"; break;
		case CmdTransmitRecordExt: ret = "CmdTransmitRecordExt"; break;
		case CmdInvalid: ret = "CmdInvalid"; break;
		case DriverInfo: ret = "DriverInfo"; break;
		default:
			unknown_command_helper = QString("Unknown command 0x%1").arg((int)cmd, 0, 16).toLatin1();
			ret = unknown_command_helper.constData();
			break;
	}
	return ret;
}

SIMessageData::MessageType SIMessageData::type() const
{
	MessageType ret = MsgInvalid;
	SIMessageData::Command cmd = command();
	if(cmd > SIMessageData::CmdInvalid) switch(cmd) {
		case SIMessageData::CmdSICard5DetectedExt:
		case SIMessageData::CmdSICard6DetectedExt:
		case SIMessageData::CmdSICard8AndHigherDetectedExt:
		case SIMessageData::CmdSICardRemovedExt:
		case SIMessageData::CmdSICardDetectedOrRemoved:
			ret = MsgCardEvent;
			break;
		case SIMessageData::CmdGetSICard5:
		case SIMessageData::CmdGetSICard5Ext:
		case SIMessageData::CmdGetSICard6:
		case SIMessageData::CmdGetSICard6Ext:
		case SIMessageData::CmdGetSICard8Ext:
			ret = MsgCardReadOut;
			break;
		case SIMessageData::CmdGetPunch2:
			ret = MsgPunch;
			break;
		case SIMessageData::DriverInfo:
			ret = MsgDriverInfo;
			break;
		default:
			ret = MsgOther;
			break;
	}
	return ret;
}

int SIMessageData::headerLength(SIMessageData::Command cmd)
{
	int ret = 0;
	switch(cmd) {
		case SIMessageData::CmdSICardRemovedExt: ret = 8; break;
		case SIMessageData::CmdSICard5DetectedExt: ret = 8; break;
		case SIMessageData::CmdSICard6DetectedExt: ret = 8; break;
		case SIMessageData::CmdSICard8AndHigherDetectedExt: ret = 8; break;
		case SIMessageData::CmdGetSICard5: ret = 2; break;
		case SIMessageData::CmdGetSICard5Ext: ret = 4; break;
		case SIMessageData::CmdGetSICard6: ret = 3; break;
		case SIMessageData::CmdGetSICard6Ext: ret = 5; break;
		case SIMessageData::CmdGetSICard8Ext: ret = 5; break;
		default:
			qfError() << "Can't find header length for command:" << cmd << SIMessageData::commandName(cmd);
			break;
	}
	return ret;
}

QByteArray SIMessageData::blockData(int block_no) const
{
	QByteArray ret = f_blockIndex.value(block_no);
	if(ret.isEmpty()) {
		qfError() << "Invalid block no:" << block_no;
	}
	return ret;
}

void SIMessageData::addRawDataBlock(const QByteArray& raw_data_with_header)
{
	int block_no_offset = 0;
	Command cmd = command(raw_data_with_header);
	//qfInfo() << "addDataBlock CMD:" << QString::number(cmd, 16);
	switch(cmd) {
		case SIMessageData::CmdSICard5DetectedExt:
		case SIMessageData::CmdSICard6DetectedExt:
		case SIMessageData::CmdSICard8AndHigherDetectedExt:
		case SIMessageData::CmdSICardRemovedExt:
			break;
		case SIMessageData::CmdGetSICard5:
		case SIMessageData::CmdGetSICard5Ext:
			break;
		case SIMessageData::CmdGetSICard6:
			block_no_offset = 2;
			break;
		case SIMessageData::CmdGetSICard6Ext:
		case SIMessageData::CmdGetSICard8Ext:
			block_no_offset = 4;
			break;
		default:
			break;
	}
	int block_no = 0;
	if(block_no_offset > 0) {
		if(block_no_offset < raw_data_with_header.length()) {
			block_no = (unsigned char)raw_data_with_header[block_no_offset];
		}
		else {
			qfError() << "Buffer is too short - offset:" << block_no_offset << "buff length:" << raw_data_with_header.length();
		}
	}
	int hdr_len = headerLength(command(raw_data_with_header));
	if(block_no == 0) {
		QByteArray hdr = raw_data_with_header.mid(0, hdr_len);
		setHeader(hdr);
	}
	QByteArray data = raw_data_with_header.mid(hdr_len);
	if(!data.isEmpty()) f_blockIndex[block_no] = data;
}

static char hex_digit(int d)
{
	char ret = '#';
	if(d < 10) ret = '0' + d;
	else ret = 'a' + (d - 10);
	return ret;
}

QString SIMessageData::dumpData(const QByteArray& ba)
{
	/*
	static const char STX = 0x02;
	static const char ETX = 0x03;
	static const char ACK = 0x06;
	static const char NAK = 0x15;
	static const char DLE = 0x10;
	*/
	QString ret = QString("length: %1 [ ").arg(ba.length());
	int i = 0;
	foreach(unsigned char c, ba) {
		QString s;
		if(i>0) ret += ' ';
		/*
		 *		if(i%16) ret += '-';
		 *		else ret += '\n';
		 *		if(c == STX) s = "ST";
		else if(c == ETX) s = "ET";
		else if(c == ACK) s = "AC";
		else if(c == NAK) s = "NA";
		else if(c == DLE) s = "DL";
		else */
		{
			char buff[] = "xx";
			buff[0] = hex_digit(c >> 4);
			buff[1] = hex_digit(c % 16);
			s = QString(buff);
		}
		ret += s;
		i++;
	}
	ret += " ]";
	return ret;
}

QString SIMessageData::dump() const
{
	QStringList sl;
	sl << QString("command: %1").arg(commandName(command()));
	sl << QString("header: %1").arg(dumpData(header()));
	QMapIterator<int, QByteArray> it(f_blockIndex);
	int block_cnt = 0;
	while(it.hasNext()) {
		it.next();
		if(!block_cnt++) continue; /// skip header
		sl << QString("------- block #%1 -------------------------------------").arg(it.key());
		QByteArray ba = it.value();
		sl << QString("data: %1").arg(dumpData(ba));
	}
	return sl.join("\n");
}
#if 0
//====================================================
//            DriverInfo
//====================================================
QString DriverInfo::dump() const
{
    QString ret = "level: %1\nmessage: %2";
	ret = ret.arg(qf::core::Log::levelName((qf::core::Log::Level)level())).arg(message());
	return ret;
}
#endif
