
//
// Author: Frantisek Vacek <fanda.vacek@volny.cz>, (C) 2011
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef SIDEVICEDRIVER_H
#define SIDEVICEDRIVER_H

#include "commport.h"

#include <siut/simessagedata.h>

#include <siut/siutglobal.h>


class QTimer;

namespace siut {


class SIUT_DECL_EXPORT DeviceDriver : public QObject
{
	Q_OBJECT
private:
	typedef QObject Super;
public:
	enum ProcessRxDataStatus {StatusUnknown, StatusMessageOk, StatusMessageError};
public:
	DeviceDriver(QObject *parent = NULL);
	virtual ~DeviceDriver();

	void processSiPacket(const QByteArray &data);
protected:
	//QSocketNotifier *f_socketNotifier;
	CommPort *f_commPort;
	QByteArray f_rxData;
	QTimer *f_rxTimer;
	ProcessRxDataStatus f_status = StatusUnknown;
	int f_packetReceivedCount = 0;
	int f_packetToFinishCount = 0;
	SIMessageData f_messageData;
protected:
	void packetReceived(const QByteArray &msg_data);
	void processRxData();
	void emitDriverInfo(qf::core::Log::Level level, const QString &msg);
public:
	bool openCommPort(const QString &device, int baudrate, int data_bits, const QString& parity, bool two_stop_bits);
	void closeCommPort();
	QString commPortErrorString();
protected slots:
	void commDataReceived();
	void rxDataTimeout();
public slots:
	void sendCommand(int cmd, const QByteArray& data);
signals:
	void driverInfo(qf::core::Log::Level level, const QString &msg);
	void messageReady(const SIMessageData &msg);
	void rawDataReceived(const QByteArray &data);
private:
	void sendAck();
	void abortMessage();
};

}

#endif // SIDEVICEDRIVER_H

