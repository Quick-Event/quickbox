
//
// Author: Frantisek Vacek <fanda.vacek@volny.cz>, (C) 2011
//
// Copyright: See COPYING file that comes with this distribution
//

#include "sidevicedriver.h"
#include "crc529.h"
#if defined Q_OS_WIN
#  include "commport_win.h"
//#  include <winbase.h>
#elif defined Q_OS_UNIX
#  include "commport_unix.h"
#else
#error "unsupported OS"
#endif

#include <simessage.h>

#include <QSocketNotifier>
#include <QTimer>
#include <QSettings>

#include <qf/core/logcust.h>

//=================================================
//             SIDeviceDriver
//=================================================
SIDeviceDriver::SIDeviceDriver(QObject *parent)
	: QThread(parent)
{
	f_commPort = NULL;
#if defined Q_OS_WIN
	f_commPort = new CommPort_win(this);
#elif defined Q_OS_UNIX
	f_commPort = new CommPort_unix(this);
#endif
	f_terminate = false;
	f_status = StatusIdle;
	//f_socketNotifier = NULL;
	f_rxTimer = new QTimer(this);
	f_rxTimer->setSingleShot(true);
	connect(f_rxTimer, SIGNAL(timeout()), this, SLOT(rxDataTimeout()));
	connect(this, SIGNAL(rawDataReceived(QByteArray)), this, SLOT(commDataReceived(QByteArray)), Qt::QueuedConnection);
	connect(f_commPort, SIGNAL(driverInfo(int,QString)), this, SIGNAL(driverInfo(int,QString)), Qt::QueuedConnection);
}

SIDeviceDriver::~SIDeviceDriver()
{
}

void SIDeviceDriver::run()
{
	qfLogFuncFrame();
	qfInfo() << "starting COM reader thread";
	while(!f_terminate) {
		if(!f_commPort->isOpen()) {
			//qfInfo() << "closed in read thread";
			break;
		}
		//qfInfo() << "read in thread";
		QByteArray ba = f_commPort->read();
		if(!ba.isEmpty()) emit rawDataReceived(ba);
	}
	qfInfo() << "exiting COM reader thread";
	emitDriverInfo(qf::core::Log::LOG_DEB, "COM reader thread exited.");
}

namespace {
	int byte_at(const QByteArray &ba, int ix)
	{
		int ret = -1;
		if(ix < ba.count()) ret = (unsigned char)ba.at(ix);
		return ret;
	}

	void set_byte_at(QByteArray &ba, int ix, unsigned char b)
	{
		ba[ix] = b;
	}
}

void SIDeviceDriver::commDataReceived(const QByteArray& ba)
{
	if(ba.size() > 0) {
		f_rxData.append(ba);
		processRxData();
		if(f_status == StatusMessageIncomplete) {
			/// set timer to get rest of the message
			f_rxTimer->start(1000);
		}
		else {
			f_rxTimer->stop();
			if(f_status == StatusMessageOk) {
				//qfInfo() << "new message:" << f_messageData.dump();
				emit messageReady(f_messageData);
				f_messageData = SIMessageData();
			}
			f_status = StatusIdle;
		}
	}
}

void SIDeviceDriver::packetReceived(const QByteArray &msg_data)
{
	qfLogFuncFrame();
	f_messageData.addRawDataBlock(msg_data);
	f_packetToFinishCount--;
	emitDriverInfo(qf::core::Log::LOG_DEB, tr("packetReceived, packetToFinishCount: %1").arg(f_packetToFinishCount));
	if(f_packetToFinishCount == 0) {
		f_status = StatusMessageOk;
	}
	else if(f_packetToFinishCount < 0) {
		f_status = StatusMessageError;
		emitDriverInfo(qf::core::Log::LOG_ERR, tr("f_packetToFinishCount < 0 - This should never happen!"));
	}
	else {
		f_status = StatusMessageIncomplete;
	}
}

namespace
{
	static const char STX = 0x02;
	static const char ETX = 0x03;
	//static const char ACK = 0x06;
	static const char NAK = 0x15;
	static const char DLE = 0x10;
}

void SIDeviceDriver::processRxData()
{
	qfLogFuncFrame();
	//ProcessRxDataStatus ret = MessageComplete;
	QSettings settings;
	/// start to find STX
	int stx_pos = 0;
	while(stx_pos < f_rxData.length()) {
		char c = f_rxData.at(stx_pos);
		//QByteArray ba1;
		//ba1.append(c);
		//qfTrash() << "processing char: " << SIMessageData::dumpData(ba1);
		if(c == STX && (stx_pos == 0 || f_rxData.at(stx_pos-1) != DLE)) {
			/// found STX
			if(stx_pos > 0) {
				qfWarning() << tr("Garbage received, stripping %1 characters from beginning of buffer").arg(stx_pos);
				f_rxData = f_rxData.mid(stx_pos);
				stx_pos = 0;
			}
			/// zacatek nove zpravy nebo pokracovani stare
			ProcessRxDataStatus prev_status = f_status;
			f_status = StatusMessageIncomplete;
			while(stx_pos < f_rxData.length()) {
				int cmd = byte_at(f_rxData, stx_pos + 1);
				//qfInfo() << "CMD:" << QString::number(cmd, 16);
				if(cmd < 0x80) {
					if(prev_status == StatusIdle) {
						if(cmd == SIMessageData::CmdGetSICard6) f_packetToFinishCount = 3;
						else f_packetToFinishCount = 1;
					}
					/// base protocol instruction (using DLE)
					stx_pos += 2; /// skip STX + CMD
					bool was_dle = false;
					QByteArray msg_data;
					msg_data.append((unsigned char)cmd);
					while(stx_pos < f_rxData.length()) {
						int ci = byte_at(f_rxData, stx_pos);
						//QByteArray ba2;
						//ba2.append((char)ci);
						//qfTrash() << "processing char: " << SIMessageData::dumpData(ba2) << stx_pos << "/" << f_rxData.length();
						if(ci < 0) {
							/// packet is not completly received
							f_status = StatusMessageIncomplete;
							return;
						}
						else if((ci == ETX || ci == NAK) && !was_dle) {
							/// whole packed received
							if(ci == NAK) {
								f_status = StatusMessageError;
								emitDriverInfo(qf::core::Log::LOG_ERR, tr("NAK received"));
							}
							else {
								packetReceived(msg_data);
							}
							f_rxData = f_rxData.mid(stx_pos+1);
							stx_pos = 0;
							break;
						}
						else if(ci == DLE) {
							if(was_dle) {
								msg_data.append((unsigned char)ci);
							}
							was_dle = !was_dle;
						}
						else {
							msg_data.append((unsigned char)ci);
							was_dle = false;
						}
						stx_pos++;
					}
				}
				else {
					/// extended mode
					if(prev_status == StatusIdle) {
						if(cmd == SIMessageData::CmdGetSICard8Ext) f_packetToFinishCount = 2; /// card 8/9/10/11
						else if(cmd == SIMessageData::CmdGetSICard6Ext) f_packetToFinishCount = 3; /// card 6
						else f_packetToFinishCount = 1;
					}
					else if(prev_status == StatusMessageIncomplete) {
						if(cmd == SIMessageData::CmdGetSICard8Ext) {
							/// cards 8/9 sends info in blocks 0,1
							/// cards 10/11 sends info in blocks 0,4,5,6,7
							/// check a BN of second packet
							int bn = byte_at(f_rxData, stx_pos + 5);
							if(bn == 4) {
								/// card 10/11
								f_packetToFinishCount += 3;
							}
							else {
								/// card 8/9
							}
						}
					}
					/// additional protocol instruction (using packet length)
					/// len - number of parameter/data bytes following, CRC excluded
					int len = byte_at(f_rxData, stx_pos + 2);
					/// CRC1,CRC0,ETX|NAK
					int status = byte_at(f_rxData, len + 5);
					if(status < 0) {
						/// packet is not completly received
						f_status = StatusMessageIncomplete;
						return;
					}
					else if(status == NAK) {
						f_status = StatusMessageError;
						emitDriverInfo(qf::core::Log::LOG_ERR, tr("NAK received"));
					}
					else {
						int crc1 = (byte_at(f_rxData, len + 3) << 8) + byte_at(f_rxData, len + 4);
						int crc2 = crc((unsigned int)len + 2, (unsigned char*)f_rxData.constData() + 1);
						if(settings.value("comm/debug/disableCRCCheck").toBool() || (crc1 == crc2)) {
							emitDriverInfo(qf::core::Log::LOG_DEB, tr("CRC check - data CRC is: %1 0x%3 computed CRC: %2 0x%4").arg(crc1).arg(crc2).arg(crc1, 0, 16).arg(crc2, 0, 16));
							/// remove transport protocol data (STX, ... msg ... , CRC1, CRC0, ETX)
							QByteArray msg_data = f_rxData.mid(1, len + 2);
							packetReceived(msg_data);
						}
						else {
							f_status = StatusMessageError;
							emitDriverInfo(qf::core::Log::LOG_ERR, tr("CRC error - data CRC is: %1 0x%3 computed CRC: %2 0x%4").arg(crc1).arg(crc2).arg(crc1, 0, 16).arg(crc2, 0, 16));
						}
					}
					f_rxData = f_rxData.mid(len + 6);
				}
			}
		}
		else {
			stx_pos++;
		}
	}
}

void SIDeviceDriver::rxDataTimeout()
{
	emitDriverInfo(qf::core::Log::LOG_ERR, tr("RX data timeout"));
	f_rxData.clear();
	f_messageData = SIMessageData();
	f_status = StatusIdle;
	//f_rxTimer->stop();
}

void SIDeviceDriver::emitDriverInfo ( int level, const QString& msg )
{
	qf::core::Log(level) << msg;
	emit driverInfo(level, msg);
}

int SIDeviceDriver::openCommPort(const QString& device, int baudrate, int data_bits, const QString& parity, bool two_stop_bits)
{
	int ret = f_commPort->open(device, baudrate, data_bits, parity, two_stop_bits);
	if(ret == 0) {
		f_terminate = false;
		start(); /// start read thread
	}
	return ret;
}

int SIDeviceDriver::closeCommPort()
{
	f_terminate = true;
	/*
	if(isRunning()) {
		#ifdef Q_OS_WIN
		HANDLE h = GetCurrentThread();
		bool cancel_io_ok = CancelSynchronousIo(h);
		qfInfo() << "canceling thread IO:" << cancel_io_ok << h;
		#endif // Q_OS_WIN
	}
	*/
	/// it is necessarry to firs terminate thread and than close COM on windows
	if(isRunning()) terminate(); /// terminate reader thread, blocking read is not interrupted by close() fd, one safer solution is to use pthread_kill
	int ret = f_commPort->close(); /// this should also terminate reader thread
	return ret;
}

void SIDeviceDriver::sendCommand(int cmd, const QByteArray& data)
{
	qfLogFuncFrame();
	if(cmd < 0x80) {
		emitDriverInfo(qf::core::Log::LOG_ERR, trUtf8("SIDeviceDriver::sendCommand() - ERROR Sending of EXT commands only is supported for sending."));
	}
	else {
		QByteArray ba;
		ba.resize(3);
		int len = data.length();
		set_byte_at(ba, 0, STX);
		set_byte_at(ba, 1, cmd);
		set_byte_at(ba, 2, len);

		ba += data;

		int crc_sum = crc(len + 2, (unsigned char*)ba.constData() + 1);
		set_byte_at(ba, ba.length(), (crc_sum >> 8) & 0xFF);
		set_byte_at(ba, ba.length(), crc_sum & 0xFF);
		set_byte_at(ba, ba.length(), ETX);
		qfInfo() << "sending command:" << SIMessageData::dumpData(ba);
		f_commPort->write(ba);
	}
}
