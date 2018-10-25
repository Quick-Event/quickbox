#include "sitasks.h"

#include <QTimer>

//===============================================================
// CmdSetDirectRemoteMode
//===============================================================
SiTaskSetDirectRemoteMode::SiTaskSetDirectRemoteMode(Mode mode, QObject *parent)
	: Super(parent)
	, m_mode(mode)
{
}

void SiTaskSetDirectRemoteMode::start()
{
	QByteArray ba(1, ' ');
	ba[0] = (m_mode == Mode::Direct)? SIMessageData::MS_MODE_DIRECT: SIMessageData::MS_MODE_REMOTE;
	sendCommand(static_cast<int>(SIMessageData::Command::SetDirectRemoteMode), ba);
}

void SiTaskSetDirectRemoteMode::onSiMessageReceived(const QByteArray &msg)
{
	bool ok = false;
	SIMessageData::Command cmd = msg.command();
	if(cmd == SIMessageData::Command::SetDirectRemoteMode) {
		QByteArray hdr = msg.dataBlock(0);
		uint8_t ms_mode = hdr[4];
		if(ms_mode == SIMessageData::MS_MODE_DIRECT) {
			qfInfo() << "SI station in DIRECT mode.";
			ok = (m_mode == Mode::Direct);
		}
		else if(ms_mode == SIMessageData::MS_MODE_REMOTE) {
			qfInfo() << "SI station in REMOTE mode.";
			ok = (m_mode == Mode::Remote);
		}
	}
	else {
		qfError() << "Invalid command:" << (int)cmd << "received";
	}
	finishAndDestroy(ok, (int)m_mode);
}

//===============================================================
// CmdStationInfo
//===============================================================
void SiTaskStationConfig::start()
{
	QByteArray ba;
	ba.append(0x74);
	ba.append(0x01);
	sendCommand(static_cast<int>(SIMessageData::Command::GetSystemData), ba);
}

void SiTaskStationConfig::onSiMessageReceived(const QByteArray &msg)
{
	bool ok = false;
	SiStationConfig ret;
	SIMessageData::Command cmd = msg.command();
	if(cmd == SIMessageData::Command::GetSystemData) {
		QByteArray hdr = msg.dataBlock(0);
		int n = hdr[2];
		n = (n << 8) + hdr[3];
		ret.setStationNumber(n);
		unsigned flags = hdr[5];
		ret.setFlags(flags);
		ok = true;
	}
	else {
		qfError() << "Invalid command:" << (int)cmd << "received";
	}
	finishAndDestroy(ok, ret);
}

//===============================================================
// CmdSetDirectRemoteMode
//===============================================================
SiTaskReadStationBackupMemory::SiTaskReadStationBackupMemory(QObject *parent)
	: Super(parent)
{
}

void SiTaskReadStationBackupMemory::start()
{
	qfInfo() << "SwitchToRemote";
	m_state = State::SwitchToRemote;
	SiTaskSetDirectRemoteMode *cmd = new SiTaskSetDirectRemoteMode(SiTaskSetDirectRemoteMode::Mode::Remote);
	connect(cmd, &SiTaskSetDirectRemoteMode::sigSendCommand, this, &SiTaskReadStationBackupMemory::sigSendCommand);
	connect(this, &SiTaskReadStationBackupMemory::siMessageForwarded, cmd, &SiTaskSetDirectRemoteMode::onSiMessageReceived);
	connect(cmd, &SiTaskSetDirectRemoteMode::finished, this, [this](bool ok, QVariant ) {
		if(ok) {
			qfInfo() << "ReadPointer";
			m_state = State::ReadPointer;
			QByteArray ba;
			ba.append(0x1C);
			ba.append(0x07);
			sendCommand(static_cast<int>(SIMessageData::Command::GetSystemData), ba);
		}
		else {
			abort();
		}
	});
	cmd->start();
}

void SiTaskReadStationBackupMemory::onSiMessageReceived(const QByteArray &msg)
{
	if(m_state == State::SwitchToRemote) {
		emit siMessageForwarded(msg);
	}
	else if(m_state == State::ReadPointer) {
		SIMessageData::Command cmd = msg.command();
		if(cmd == SIMessageData::Command::GetSystemData) {
			QByteArray hdr = msg.dataBlock(0);
			int ix = 5;
			/// the 4 byte backup memory address pointer is part of the data string: EP3, EP2, xx, xx, xx, EP1, EP0
			m_memoryDataPointer = hdr[ix++];
			m_memoryDataPointer = (m_memoryDataPointer << 8) +  hdr[ix++];
			ix += 3;
			m_memoryDataPointer = (m_memoryDataPointer << 8) +  hdr[ix++];
			m_memoryDataPointer = (m_memoryDataPointer << 8) +  hdr[ix++];
			qfInfo() << "backup memory pointer:" << "0x" + QString::number(m_memoryDataPointer, 16);

			m_state = State::CheckOverflow;
			/// according to
			/// https://github.com/gaudenz/sireader/blob/master/sireader.py
			/// we should find ad addr 0x3D
			qfInfo() << "CheckOverflow";
			QByteArray ba;
			ba.append(0x3D);
			ba.append(0x01);
			sendCommand(static_cast<int>(SIMessageData::Command::GetSystemData), ba);
		}
		else {
			qfError() << "Invalid command:" << (int)cmd << "received";
			abort();
			return;
		}
	}
	else if(m_state == State::CheckOverflow) {
		SIMessageData::Command cmd = msg.command();
		if(cmd == SIMessageData::Command::GetSystemData) {
			QByteArray hdr = msg.dataBlock(0);
			m_isOverflow = hdr[5];
			//qfInfo().noquote() << msg.dump();
			qfInfo() << "is memory overflow:" << m_isOverflow;

			m_blockCount = m_isOverflow? MEMORY_SIZE: (m_memoryDataPointer - 0x100) / m_blockSize + 1;
			if(m_blockCount == 0) {
				finishAndDestroy(true, m_data);
				return;
			}
			if(m_blockCount > 255) {
				qfError() << "Invalid block count:" << m_blockCount;
				abort();
				return;
			}

			qfInfo() << "ReadData";
			m_state = State::ReadData;
			QByteArray ba;
			if(m_isOverflow)
				m_readDataPointer = m_memoryDataPointer + 1;
			else
				m_readDataPointer = MEMORY_START;
			ba.append((m_readDataPointer >> (2*8)) & 0xFF);
			ba.append((m_readDataPointer >> (1*8)) & 0xFF);
			ba.append((m_readDataPointer >> (0*8)) & 0xFF);
			ba.append(m_blockSize);
			sendCommand(0x81, ba);
		}
		else {
			qfError() << "Invalid command:" << (int)cmd << "received";
			abort();
		}
	}
	else if(m_state == State::ReadData) {
		int cmd = (int)msg.command();
		if(cmd == 0x81) {
			qfInfo().noquote() << msg.dump();
			QByteArray ba = msg.dataBlock(0).mid(7);
			if(m_readDataPointer < m_memoryDataPointer) {
				if(m_memoryDataPointer - m_readDataPointer < m_blockSize)
					ba = ba.mid(0, m_memoryDataPointer - m_readDataPointer);
			}
			m_data.append(ba);
			if(!--m_blockCount) {
				qfInfo() << "SwitchToDirect";
				m_state = State::SwitchToDirect;
				SiTaskSetDirectRemoteMode *cmd = new SiTaskSetDirectRemoteMode(SiTaskSetDirectRemoteMode::Mode::Direct);
				connect(cmd, &SiTaskSetDirectRemoteMode::sigSendCommand, this, &SiTaskReadStationBackupMemory::sigSendCommand);
				connect(this, &SiTaskReadStationBackupMemory::siMessageForwarded, cmd, &SiTaskSetDirectRemoteMode::onSiMessageReceived);
				connect(cmd, &SiTaskSetDirectRemoteMode::finished, this, [this](bool ok, QVariant ) {
					if(ok) {
						finishAndDestroy(true, m_data);
					}
					else {
						abort();
					}
				});
				cmd->start();
				return;
			}
			m_readDataPointer += m_blockSize;
			if(m_readDataPointer - MEMORY_START > MEMORY_SIZE)
				m_readDataPointer = 0x100;
			{
				QByteArray ba;
				ba.append((m_readDataPointer >> (2*8)) & 0xFF);
				ba.append((m_readDataPointer >> (1*8)) & 0xFF);
				ba.append((m_readDataPointer >> (0*8)) & 0xFF);
				ba.append(m_blockSize);
				sendCommand(0x81, ba);
			}
		}
		else {
			qfError() << "Invalid command:" << cmd << "received";
			abort();
			return;
		}
	}
	else if(m_state == State::SwitchToDirect) {
		emit siMessageForwarded(msg);
	}
}


