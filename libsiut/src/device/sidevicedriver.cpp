
//
// Author: Frantisek Vacek <fanda.vacek@volny.cz>, (C) 2011
//
// Copyright: See COPYING file that comes with this distribution
//

#include "sidevicedriver.h"
#include "crc529.h"

#include <siut/simessage.h>

#include <qf/core/log.h>

#include <QTimer>
#include <QSettings>
#include <QSerialPortInfo>

using namespace siut;

//=================================================
//             DeviceDriver
//=================================================
DeviceDriver::DeviceDriver(QObject *parent)
	: Super(parent), f_packetToFinishCount()
{
	qf::core::Log::checkLogLevelMetaTypeRegistered();
	f_commPort = new CommPort(this);
	f_status = StatusIdle;
	//f_socketNotifier = NULL;
	f_rxTimer = new QTimer(this);
	f_rxTimer->setSingleShot(true);
	connect(f_rxTimer, SIGNAL(timeout()), this, SLOT(rxDataTimeout()));
	connect(f_commPort, SIGNAL(readyRead()), this, SLOT(commDataReceived()));
	connect(f_commPort, &CommPort::driverInfo, this, &DeviceDriver::driverInfo, Qt::QueuedConnection);
}

DeviceDriver::~DeviceDriver()
{
	if(f_commPort->isOpen())
		f_commPort->close();
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

void DeviceDriver::commDataReceived()
{
	QByteArray ba = f_commPort->readAll();
	//qfInfo() << "=============>" << ba;
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
				if(f_messageData.type() == SIMessageData::MsgCardReadOut) {
					sendAck();
				}
				emit messageReady(f_messageData);
				f_messageData = SIMessageData();
			}
			f_status = StatusIdle;
		}
	}
}

void DeviceDriver::packetReceived(const QByteArray &msg_data)
{
	qfLogFuncFrame();
	//qWarning() << msg_data;
	emit rawDataReceived(msg_data);
	f_messageData.addRawDataBlock(msg_data);
	f_packetToFinishCount--;
	emitDriverInfo(qf::core::Log::Level::Debug, tr("packetReceived, packetToFinishCount: %1").arg(f_packetToFinishCount));
	if(f_packetToFinishCount == 0) {
		f_status = StatusMessageOk;
	}
	else if(f_packetToFinishCount < 0) {
		f_status = StatusMessageError;
		emitDriverInfo(qf::core::Log::Level::Error, tr("f_packetToFinishCount < 0 - This should never happen!"));
	}
	else {
		f_status = StatusMessageIncomplete;
	}
}

namespace
{
static const char STX = 0x02;
static const char ETX = 0x03;
static const char ACK = 0x06;
static const char NAK = 0x15;
static const char DLE = 0x10;
}

void DeviceDriver::processRxData()
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
						if(cmd == SIMessageData::CmdGetSICard6)
							f_packetToFinishCount = 3;
						else
							f_packetToFinishCount = 1;
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
								emitDriverInfo(qf::core::Log::Level::Error, tr("NAK received"));
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
						if(cmd == SIMessageData::CmdGetSICard8Ext)
							f_packetToFinishCount = 2; /// card 8/9/10/11
						else if(cmd == SIMessageData::CmdGetSICard6Ext)
							f_packetToFinishCount = 3; /// card 6
						else
							f_packetToFinishCount = 1;
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
						emitDriverInfo(qf::core::Log::Level::Error, tr("NAK received"));
					}
					else {
						int crc1 = (byte_at(f_rxData, len + 3) << 8) + byte_at(f_rxData, len + 4);
						int crc2 = crc((unsigned int)len + 2, (unsigned char*)f_rxData.constData() + 1);
						if(settings.value("comm/debug/disableCRCCheck").toBool() || (crc1 == crc2)) {
							emitDriverInfo(qf::core::Log::Level::Debug, tr("CRC check - data CRC is: %1 0x%3 computed CRC: %2 0x%4").arg(crc1).arg(crc2).arg(crc1, 0, 16).arg(crc2, 0, 16));
							/// remove transport protocol data (STX, ... msg ... , CRC1, CRC0, ETX)
							QByteArray msg_data = f_rxData.mid(1, len + 2);
							packetReceived(msg_data);
						}
						else {
							f_status = StatusMessageError;
							emitDriverInfo(qf::core::Log::Level::Error, tr("CRC error - data CRC is: %1 0x%3 computed CRC: %2 0x%4").arg(crc1).arg(crc2).arg(crc1, 0, 16).arg(crc2, 0, 16));
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

void DeviceDriver::rxDataTimeout()
{
	emitDriverInfo(qf::core::Log::Level::Error, tr("RX data timeout"));
	f_rxData.clear();
	f_messageData = SIMessageData();
	f_status = StatusIdle;
	//f_rxTimer->stop();
}

void DeviceDriver::emitDriverInfo ( qf::core::Log::Level level, const QString& msg )
{
	//qfLog(level) << msg;
	emit driverInfo(level, msg);
}

bool DeviceDriver::openCommPort(const QString& _device, int baudrate, int data_bits, const QString& parity_str, bool two_stop_bits)
{
	qfLogFuncFrame();
	QString device = _device;
	{
		qfDebug() << "Port enumeration";
		QList<QSerialPortInfo> port_list = QSerialPortInfo::availablePorts();
		QStringList sl;
		for(auto port : port_list) {
			if(device.isEmpty()) device = port.systemLocation();
			sl << QString("%1 %2").arg(port.portName()).arg(port.systemLocation());
			qfDebug() << "\t" << port.portName();
		}
		emitDriverInfo(qf::core::Log::Level::Info, trUtf8("Available ports: %1").arg(sl.join(QStringLiteral(", "))));
	}
	f_commPort->setPortName(device);
	f_commPort->setBaudRate(baudrate);
	f_commPort->setDataBitsAsInt(data_bits);
	f_commPort->setParityAsString(parity_str);
	f_commPort->setStopBits(two_stop_bits? QSerialPort::TwoStop: QSerialPort::OneStop);
	//f_commPort->setFlowControl(p.flowControl);
	emitDriverInfo(qf::core::Log::Level::Debug, trUtf8("Connecting to %1 - baudrate: %2, data bits: %3, parity: %4, stop bits: %5")
				   .arg(f_commPort->portName())
				   .arg(f_commPort->baudRate())
				   .arg(f_commPort->dataBits())
				   .arg(f_commPort->parity())
				   .arg(f_commPort->stopBits())
				   );
	bool ret = f_commPort->open(QIODevice::ReadWrite);
	if(ret) {
		emitDriverInfo(qf::core::Log::Level::Info, trUtf8("%1 connected OK").arg(device));
	}
	else {
		emitDriverInfo(qf::core::Log::Level::Error, trUtf8("%1 connect ERROR: %2").arg(device).arg(f_commPort->errorString()));
	}
	return ret;
}

void DeviceDriver::closeCommPort()
{
	if(f_commPort->isOpen()) {
		f_commPort->close();
		emitDriverInfo(qf::core::Log::Level::Info, trUtf8("%1 closed").arg(f_commPort->portName()));
	}
}

void DeviceDriver::sendCommand(int cmd, const QByteArray& data)
{
	qfLogFuncFrame();
	if(cmd < 0x80) {
		emitDriverInfo(qf::core::Log::Level::Error, trUtf8("SIDeviceDriver::sendCommand() - ERROR Sending of EXT commands only is supported for sending."));
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

void DeviceDriver::sendAck() 
{
	f_commPort->write(QByteArray(1, ACK));
}
