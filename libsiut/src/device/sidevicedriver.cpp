
//
// Author: Frantisek Vacek <fanda.vacek@volny.cz>, (C) 2011
//
// Copyright: See COPYING file that comes with this distribution
//

#include "sidevicedriver.h"
#include "crc529.h"

//#include <siut/simessage.h>

#include <qf/core/log.h>

#include <QTimer>
#include <QSettings>
#include <QSerialPortInfo>

namespace siut {

//=================================================
//             DeviceDriver
//=================================================
DeviceDriver::DeviceDriver(QObject *parent)
	: Super(parent)
{
	qf::core::Log::checkLogLevelMetaTypeRegistered();
}

DeviceDriver::~DeviceDriver()
{
}

namespace {
/*
	int byte_at(const QByteArray &ba, int ix)
	{
		int ret = -1;
		if(ix < ba.count()) ret = (unsigned char)ba.at(ix);
		return ret;
	}
*/
	void set_byte_at(QByteArray &ba, int ix, char b)
	{
		ba[ix] = b;
	}
}

void DeviceDriver::processSIMessageData(const SIMessageData &data)
{
	qfLogFuncFrame();
	qfDebug().noquote() << data.toString();
	if(m_taskInProcess) {
		m_taskInProcess->onSiMessageReceived(data);
		return;
	}
	SIMessageData::Command cmd = data.command();
	switch(cmd) {
	case SIMessageData::Command::SICardRemoved: {
		qfInfo() << "SICardRemoved";
		break;
	}
	case SIMessageData::Command::SICard5Detected: {
		qfInfo() << "SICard5Detected";
		setSiTask(new SiTaskReadCard5(false));
		break;
	}
	case SIMessageData::Command::GetSICard5: {
		setSiTask(new SiTaskReadCard5(true));
		processSIMessageData(data);
		break;
	}
	case SIMessageData::Command::SICard6Detected: {
		qfInfo() << "SICard6Detected";
		setSiTask(new SiTaskReadCard6(false));
		break;
	}
	case SIMessageData::Command::GetSICard6: {
		setSiTask(new SiTaskReadCard6(true));
		processSIMessageData(data);
		break;
	}
	case SIMessageData::Command::SICard8Detected: {
		qfInfo() << "SICard8AndHigherDetected";
		setSiTask(new SiTaskReadCard8(false));
		break;
	}
	case SIMessageData::Command::GetSICard8: {
		setSiTask(new SiTaskReadCard8(true));
		processSIMessageData(data);
		break;
	}
	default:
		qfError() << "unsupported command" << QString::number((uint8_t)cmd, 16);
	}
}

namespace
{
static const char STX = 0x02;
static const char ETX = 0x03;
//static const char ACK = 0x06;
static const char NAK = 0x15;
//static const char DLE = 0x10;
}

void DeviceDriver::processData(const QByteArray &data)
{
	qfLogFuncFrame() << "\n" << SIMessageData::dumpData(data, 16);
	f_rxData.append(data);
	while(f_rxData.size() > 3) {
		int stx_pos = f_rxData.indexOf(STX);
		if(stx_pos > 0)
			qfWarning() << tr("Garbage received, stripping %1 characters from beginning of buffer").arg(stx_pos);
		// remove multiple STX, this can happen
		while(stx_pos < f_rxData.size()-1 && f_rxData[stx_pos+1] == STX)
			stx_pos++;
		if(stx_pos > 0) {
			f_rxData = f_rxData.mid(stx_pos);
			stx_pos = 0;
		}
		// STX,CMD,LEN, data, CRC1,CRC0,ETX/NAK
		if(f_rxData.size() < 3) // STX,CMD,LEN
			return;
		int len = (uint8_t)f_rxData[2];
		len += 3 + 3;
		if(f_rxData.size() < len)
			return;
		uint8_t etx = (uint8_t)f_rxData[len-1];
		if(etx == NAK) {
			emitDriverInfo(qf::core::Log::Level::Error, tr("NAK received"));
		}
		else if(etx == ETX) {
			QByteArray data = f_rxData.mid(0, len);
			uint8_t cmd = (uint8_t)data[1];
			if(cmd < 0x80) {
				emitDriverInfo(qf::core::Log::Level::Error, tr("Legacy protocol is not supported, switch station to extended one."));
			}
			else {
				processSIMessageData(data);
			}
		}
		else {
			qfWarning() << tr("Valid message shall end with ETX or NAK, throwing data away");
		}
		f_rxData = f_rxData.mid(len);
	}
#if 0
	while(f_rxData.size() && f_rxData[0] == STX)
		f_rxData = f_rxData.mid(1);
	{
		QSettings settings;
		/// start to find STX
		int stx_pos = 0;
		while(stx_pos < f_rxData.length()) {
			char c = f_rxData.at(stx_pos);
			if(c == STX && (stx_pos == 0 || f_rxData.at(stx_pos-1) != DLE)) {
				/// found STX
				if(stx_pos > 0) {
					qfWarning() << tr("Garbage received, stripping %1 characters from beginning of buffer").arg(stx_pos);
					f_rxData = f_rxData.mid(stx_pos);
					stx_pos = 0;
				}
				/// zacatek nove zpravy nebo pokracovani stare
				while(stx_pos < f_rxData.length()) {
					int cmd = byte_at(f_rxData, stx_pos + 1);
					//qfInfo() << "CMD:" << QString::number(cmd, 16);
					if(cmd < 0x80) { /// legacy protocol
						if(f_packetReceivedCount == 0) {
							if(cmd == static_cast<int>(SIMessageData::Command::GetSICard6))
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
							if(ci < 0) {
								/// packet is not completly received
								return;
							}
							else if((ci == ETX || ci == NAK) && !was_dle) {
								/// whole packed received
								if(ci == NAK) {
									abortMessage();
									emitDriverInfo(qf::core::Log::Level::Error, tr("NAK received"));
								}
								else {
									processReceivedSiDatagram(msg_data);
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
					else { /// extended mode
						if(f_packetReceivedCount == 0) {
							if(cmd == (int)SIMessageData::Command::GetSICard8Ext)
								f_packetToFinishCount = 2; /// card 8/9/10/11
							else if(cmd == (int)SIMessageData::Command::GetSICard6Ext)
								f_packetToFinishCount = 3; /// card 6
							else
								f_packetToFinishCount = 1;
						}
						else if(f_packetReceivedCount == 1) {
							if(cmd == (int)SIMessageData::Command::GetSICard8Ext) {
								/// cards 8/9 sends info in blocks 0,1
								/// cards 10/11 sends info in blocks 0,4,5,6,7
								/// check a BN of second packet
								int bn = byte_at(f_rxData, stx_pos + 5);
								if(bn == 4) {
									/// card 10/11
									f_packetToFinishCount += 3;
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
							return;
						}
						else if(status == NAK) {
							abortMessage();
							emitDriverInfo(qf::core::Log::Level::Error, tr("NAK received"));
						}
						else {
							int crc1 = (byte_at(f_rxData, len + 3) << 8) + byte_at(f_rxData, len + 4);
							int crc2 = crc((unsigned int)len + 2, (unsigned char*)f_rxData.constData() + 1);
							if(settings.value("comm/debug/disableCRCCheck").toBool() || (crc1 == crc2)) {
								emitDriverInfo(qf::core::Log::Level::Debug, tr("CRC check - data CRC is: %1 0x%3 computed CRC: %2 0x%4").arg(crc1).arg(crc2).arg(crc1, 0, 16).arg(crc2, 0, 16));
								/// remove transport protocol data (STX, ... msg ... , CRC1, CRC0, ETX)
								QByteArray msg_data = f_rxData.mid(1, len + 2);
								processReceivedSiDatagram(msg_data);
							}
							else {
								abortMessage();
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
#endif
}
/*
void DeviceDriver::onSiMessageReceived(const SIMessageData &msg)
{
	if(m_taskInProcess) {
		m_taskInProcess->onSiMessageReceived(msg);
		return;
	}
	if(msg.type() == SIMessageData::MessageType::CardReadOut) {
		sendAck();
	}
	emit siMessageReceived(msg);
}
*/
void DeviceDriver::emitDriverInfo ( qf::core::Log::Level level, const QString& msg )
{
	//qfLog(level) << msg;
	emit driverInfo(level, msg);
}
#if 0
void DeviceDriver::rxDataTimeout()
{
	emitDriverInfo(qf::core::Log::Level::Error, tr("RX data timeout"));
	f_rxData.clear();
	f_messageData = SIMessageData();
	abortMessage();
}

bool DeviceDriver::openCommPort(const QString& _device, int baudrate, int data_bits, const QString& parity_str, bool two_stop_bits)
{
	qfLogFuncFrame();
	closeCommPort();
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
		emit commOpenChanged(true);
		emitDriverInfo(qf::core::Log::Level::Info, trUtf8("%1 connected OK").arg(device));
	}
	else {
		emit commOpenChanged(false);
		emitDriverInfo(qf::core::Log::Level::Error, trUtf8("%1 connect ERROR: %2").arg(device).arg(f_commPort->errorString()));
	}
	return ret;
}

void DeviceDriver::closeCommPort()
{
	if(f_commPort->isOpen()) {
		f_commPort->close();
		emit commOpenChanged(false);
		emitDriverInfo(qf::core::Log::Level::Info, trUtf8("%1 closed").arg(f_commPort->portName()));
	}
}

QString DeviceDriver::commPortErrorString()
{
	return f_commPort->errorString();
}

QString DeviceDriver::commPortName()
{
	return f_commPort->portName();
}
#endif
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
		set_byte_at(ba, 1, (char)cmd);
		set_byte_at(ba, 2, (char)len);

		ba += data;

		int crc_sum = crc(len + 2, (unsigned char*)ba.constData() + 1);
		set_byte_at(ba, ba.length(), (crc_sum >> 8) & 0xFF);
		set_byte_at(ba, ba.length(), crc_sum & 0xFF);
		set_byte_at(ba, ba.length(), ETX);
		qfDebug().noquote() << "sending command:" << SIMessageData::dumpData(ba, 16);
		//f_commPort->write(ba);
		//f_rxTimer->start();
		emit dataToSend(ba);
	}
}

void DeviceDriver::setSiTask(SiTask *task)
{
	if(m_taskInProcess) {
		qfError() << "There is other command in progress already. It will be aborted and deleted.";
		m_taskInProcess->abort();
	}
	m_taskInProcess = task;
	SiTask::Type task_type = task->type();
	connect(task, &SiTask::sigSendCommand, this, &DeviceDriver::sendCommand);
	connect(task, &SiTask::aboutToFinish, this, [this]() {
		this->m_taskInProcess = nullptr;
	});
	connect(task, &SiTask::finished, this, [this, task_type](bool ok, QVariant result) {
		if(ok) {
			emit this->siTaskFinished(static_cast<int>(task_type), result);
		}
	});
	m_taskInProcess->start();
}
/*
void DeviceDriver::sendAck() 
{
	emit dataToSend(QByteArray(1, ACK));
}

void DeviceDriver::abortMessage()
{
	f_status = StatusMessageError;
	f_packetReceivedCount = 0;
	f_packetToFinishCount = 0;
}
*/
}
