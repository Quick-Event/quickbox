
//
// Author: Frantisek Vacek <fanda.vacek@volny.cz>, (C) 2011
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef SIUT_SIMESSAGEDATA_H
#define SIUT_SIMESSAGEDATA_H

#include <siut/siutglobal.h>

#include <QVariantMap>

namespace siut {

class SIUT_DECL_EXPORT SIMessageData
{
public:
	static constexpr uint8_t MS_MODE_DIRECT = 0x4D;
	static constexpr uint8_t MS_MODE_REMOTE = 0x53;
	enum class Command {
		Invalid=0,
		//SICardDetectedOrRemoved='F', /// next byte shoul be 'I' (detect) or 'O' (removed)
		GetBackupMemory=0x81,
		GetSystemData=0x83,
		SICard5Detected=0xE5,
		SICard6Detected=0xE6,
		SICard8Detected=0xE8,
		SICardRemoved=0xE7,
		GetSICard5=0xB1,
		GetSICard6=0xE1,
		GetSICard8=0xEF,
		//GetPunch2=0x53, /// autosend only (ie. punch)
		SetDirectRemoteMode=0xF0,
		//TimeSend=0x54, /// autosend only (ie. trigger data)
		TransmitRecord=0xD3, /// autosend only (transmit punch or trigger data)

		//DriverInfo=0x1000 /// Driver info (SI commands are only 8 bit long)
	};
	//enum class MessageType {Invalid=0, CardEvent, CardReadOut, Punch, DriverInfo, Other};
public:
	SIMessageData() {}
	SIMessageData(const QByteArray &si_data) : m_data(si_data) {}
	virtual ~SIMessageData() {}
public:
	bool isNull() const {return m_data.isEmpty();}
	static Command command(const QByteArray &si_data);
	Command command() const {return command(data());}
	//MessageType type() const;
	/// offset of data in packet for each command
	//static int headerLength(Command);
	const QByteArray& data() const {return m_data;}
	virtual QString toString(int bytes_in_the_row = 4) const;
	static const char* commandName(Command cmd);
	static QString dumpData(const QByteArray &ba, int bytes_in_the_row);
private:
	QByteArray m_data; ///< block_no->rawData
};

}

Q_DECLARE_METATYPE(siut::SIMessageData)

#endif // SIMESSAGEDATA_H

