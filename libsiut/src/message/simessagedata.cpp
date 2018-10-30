
//
// Author: Frantisek Vacek <fanda.vacek@volny.cz>, (C) 2011
//
// Copyright: See COPYING file that comes with this distribution
//

#include "simessagedata.h"

#include <qf/core/log.h>

#include <QStringList>

namespace siut {
/*
namespace
{
	class SIMessageDataMetaTypeInit
	{
	public:
		SIMessageDataMetaTypeInit()
		{
			qRegisterMetaType<siut::SIMessageData>("SIMessageData");
		}
	};

	SIMessageDataMetaTypeInit initializer;
}
*/
//=================================================
//             SIMessageData
//=================================================
SIMessageData::Command SIMessageData::command(const QByteArray &si_data)
{
	Command ret = Command::Invalid;
	if(si_data.length() > 1) {
		ret = (Command)(unsigned char)si_data[1];
	}
	else {
		qfError() << "invalid message";
		//qfInfo() << qf::core::Log::stackTrace();
	}
	return ret;
}

const char* SIMessageData::commandName(SIMessageData::Command cmd)
{
	const char *ret;
	switch(cmd) {
		case Command::SICard5Detected: ret = "CmdSICard5Detected"; break;
		case Command::SICard6Detected: ret = "CmdSICard6Detected"; break;
		case Command::SICard8AndHigherDetected: ret = "CmdSICard8+"; break;
		case Command::SICardRemoved: ret = "CmdSICardRemoved"; break;
		//case Command::SICardDetectedOrRemoved: ret = "CmdSICardDetectedOrRemoved"; break;
		case Command::GetSICard5: ret = "CmdGetSICard5"; break;
		case Command::GetSICard6: ret = "CmdGetSICard6"; break;
		case Command::GetSICard8: ret = "CmdGetSICard8"; break;
		//case Command::GetPunch2: ret = "CmdGetPunch2"; break;
		//case Command::TimeSend: ret = "CmdTimeSend"; break;
		case Command::TransmitRecord: ret = "CmdTransmitRecord"; break;
		case Command::Invalid: ret = "CmdInvalid"; break;
		//case Command::DriverInfo: ret = "DriverInfo"; break;
		default:
			ret = "UnknownCommand";
			break;
	}
	return ret;
}

SIMessageData::MessageType SIMessageData::type() const
{
	MessageType ret = MessageType::Invalid;
	SIMessageData::Command cmd = command();
	if(cmd > SIMessageData::Command::Invalid)
		switch(cmd) {
		case SIMessageData::Command::SICard5Detected:
		case SIMessageData::Command::SICard6Detected:
		case SIMessageData::Command::SICard8AndHigherDetected:
		case SIMessageData::Command::SICardRemoved:
		//case SIMessageData::Command::SICardDetectedOrRemoved:
		//	ret = MessageType::CardEvent;
		//	break;
		case SIMessageData::Command::GetSICard5:
		case SIMessageData::Command::GetSICard6:
		case SIMessageData::Command::GetSICard8:
			ret = MessageType::CardReadOut;
			break;
		//case SIMessageData::Command::GetPunch2:
		case SIMessageData::Command::TransmitRecord:
			ret = MessageType::Punch;
			break;
		//case SIMessageData::Command::DriverInfo:
		//	ret = MessageType::DriverInfo;
		//	break;
		default:
			ret = MessageType::Other;
			break;
		}
	return ret;
}
/*
int SIMessageData::headerLength(SIMessageData::Command cmd)
{
	int ret = 0;
	switch(cmd) {
	case SIMessageData::Command::GetSystemData: ret = 0; break; //depends on type of data queried
	case SIMessageData::Command::SetDirectRemoteMode: ret = 5; break;
	case SIMessageData::Command::SICardRemovedExt: ret = 8; break;
	case SIMessageData::Command::SICard5DetectedExt: ret = 8; break;
	case SIMessageData::Command::SICard6DetectedExt: ret = 8; break;
	case SIMessageData::Command::SICard8AndHigherDetectedExt: ret = 8; break;
	case SIMessageData::Command::GetSICard5: ret = 2; break;
	case SIMessageData::Command::GetSICard5Ext: ret = 4; break;
	case SIMessageData::Command::GetSICard6: ret = 3; break;
	case SIMessageData::Command::GetSICard6Ext: ret = 5; break;
	case SIMessageData::Command::GetSICard8Ext: ret = 5; break;
	case SIMessageData::Command::TransmitRecordExt: ret = 15; break;
	default:
		qfError() << "Can't find header length for command:" << (int)cmd << SIMessageData::commandName(cmd);
		break;
	}
	return ret;
}

QByteArray SIMessageData::dataBlock(int block_ix) const
{
	QByteArray ret = m_data.value(block_ix);
	if(ret.isEmpty()) {
		qfError() << "Invalid block no:" << block_ix;
	}
	return ret;
}
*/
#if 0
void SIMessageData::addRawDataBlock(const QByteArray& si_data_block)
{
	m_data << si_data_block;
	/*
	int block_no_offset = 0;
	Command cmd = command(raw_data_with_header);
	//qfInfo() << "addDataBlock CMD:" << QString::number(cmd, 16);
	switch(cmd) {
		case SIMessageData::Command::SICard5DetectedExt:
		case SIMessageData::Command::SICard6DetectedExt:
		case SIMessageData::Command::SICard8AndHigherDetectedExt:
		case SIMessageData::Command::SICardRemovedExt:
			break;
		case SIMessageData::Command::GetSICard5:
		case SIMessageData::Command::GetSICard5Ext:
			break;
		case SIMessageData::Command::GetSICard6:
			block_no_offset = 2;
			break;
		case SIMessageData::Command::GetSICard6Ext:
		case SIMessageData::Command::GetSICard8Ext:
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
		QByteArray hdr;
		if(hdr_len > 0)
			hdr = raw_data_with_header.mid(0, hdr_len);
		else
			hdr = raw_data_with_header;
		setHeader(raw_data_with_header);
	}
	QByteArray data = raw_data_with_header.mid(hdr_len);
	if(!data.isEmpty())
		f_blockIndex[block_no] = data;
	*/
}
#endif
static char hex_digit(int d)
{
	char ret = '#';
	if(d < 10) ret = '0' + d;
	else ret = 'a' + (d - 10);
	return ret;
}

QString SIMessageData::dumpData(const QByteArray& ba, int bytes_in_the_row)
{
	/*
	static constexpr char STX = 0x02;
	static constexpr char ETX = 0x03;
	static constexpr char ACK = 0x06;
	static constexpr char NAK = 0x15;
	static constexpr char DLE = 0x10;
	*/
	QString ret = QString("data length: %1").arg(ba.length());
	int i = 0;
	int page = 0;
	for(unsigned char c : ba) {
		QString s;
		if(i % bytes_in_the_row == 0) {
			ret += '\n';
			ret += QString("%1 ").arg(page++, 2, 10, QChar('0'));
			ret += QString("%1 ").arg(i, 4, 16, QChar('0'));
		}
		/*
		if(c == STX)
			s = "SX";
		else if(c == ETX)
			s = "EX";
		else if(c == ACK)
			s = "AK";
		else if(c == NAK)
			s = "NK";
		else if(c == DLE)
			s = "DL";
		else */
		{
			char buff[] = "xx ";
			buff[0] = hex_digit(c >> 4);
			buff[1] = hex_digit(c % 16);
			s = QString(buff);
		}
		ret += s;
		i++;
	}
	return ret;
}

QString SIMessageData::toString(int bytes_in_the_row) const
{
	QString ret;
	ret = QString("command: %1\n").arg(commandName(command()));
	ret += dumpData(m_data, bytes_in_the_row);
	return ret;
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

}
