
//
// Author: Frantisek Vacek <fanda.vacek@volny.cz>, (C) 2011
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef SIMESSAGEDATA_H
#define SIMESSAGEDATA_H

#include <siut/siutglobal.h>

#include <QVariantMap>
#include <QSharedData>

class SIUT_DECL_EXPORT SIMessageData
{
public:
	enum Command {
		CmdInvalid=0,
		CmdSICardDetectedOrRemoved='F', /// next byte shoul be 'I' (detect) or 'O' (removed)
		CmdSICard5DetectedExt=0xE5,
		CmdSICard6DetectedExt=0xE6,
		CmdSICard8AndHigherDetectedExt=0xE8,
		CmdSICardRemovedExt=0xE7,
		CmdGetSICard5=0x31, CmdGetSICard5Ext=0xB1,
		CmdGetSICard6=0x61, CmdGetSICard6Ext=0xE1,
		CmdGetSICard8Ext=0xEF,
		CmdGetPunch2=0x53, /// autosend only (ie. punch)
		CmdTimeSend=0x54, /// autosend only (ie. trigger data)
		CmdTransmitRecordExt=0xD3, /// autosend only (transmit punch or trigger data)

		DriverInfo=0x1000 /// Driver info (SI commands are only 8 bit long)
	};
	enum MessageType {MsgInvalid=0, MsgCardEvent, MsgCardReadOut, MsgPunch, MsgDriverInfo, MsgOther};
public:
	SIMessageData() {}
	virtual ~SIMessageData() {}
public:
	bool isNull() const {return f_blockIndex.isEmpty();}
	static Command command(const QByteArray &raw_data_with_header);
	Command command() const {return command(header());}
	MessageType type() const;
	/// offset of data in packet for each command
	static int headerLength(Command);
	QByteArray header() const {return f_blockIndex.value(-1);}
	void setHeader(const QByteArray &h) {f_blockIndex[-1] = h;}
	/// returns datablock (message header is skipped)
	/// first datablock has block_no 0
	QByteArray blockData(int block_no) const;
	QList<int> blockNumbers() const {return f_blockIndex.keys().mid(1);}
	virtual QString dump() const;
	static const char* commandName(Command cmd);
	static QString dumpData(const QByteArray &ba);
	void addRawDataBlock(const QByteArray &raw_data_with_header);
private:
	QMap<int, QByteArray> f_blockIndex; ///< block_no->rawData
};
Q_DECLARE_METATYPE(SIMessageData)
#if 0
class DriverInfo : public SIMessageBase
{
public:
	/// same as QFLog::Level
	int level() const {return f_data.value("level").toInt();}
	QString message() const {return f_data.value("message").toString();}
	virtual QString dump() const;
public:
    DriverInfo(/*QFLog::Level*/int level, const QString &message) {
		f_data["level"] = level;
		f_data["message"] = message;
	}
};
#endif
#endif // SIMESSAGEDATA_H

