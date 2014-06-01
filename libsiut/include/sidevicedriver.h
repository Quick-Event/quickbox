
//
// Author: Frantisek Vacek <fanda.vacek@volny.cz>, (C) 2011
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef SIDEVICEDRIVER_H
#define SIDEVICEDRIVER_H

#include <commport.h>
#include <simessagedata.h>

#include <siutglobal.h>

#include <QThread>

class QTimer;

//! TODO: write class documentation.
class SIUT_DECL_EXPORT SIDeviceDriver : public QThread
{
	Q_OBJECT;
public:
	enum ProcessRxDataStatus {StatusIdle, StatusMessageIncomplete, StatusMessageOk, StatusMessageError};
protected:
	//QSocketNotifier *f_socketNotifier;
	CommPort *f_commPort;
	QByteArray f_rxData;
	QTimer *f_rxTimer;
	ProcessRxDataStatus f_status;
	int f_packetToFinishCount;
	SIMessageData f_messageData;
	bool f_terminate;
protected:
	virtual void run();
	void packetReceived(const QByteArray &msg_data);
	void processRxData();
	void emitDriverInfo(int level, const QString &msg);
public:
	int openCommPort(const QString &device, int baudrate, int data_bits, const QString& parity, bool two_stop_bits);
	int closeCommPort();
protected slots:
	void commDataReceived(const QByteArray &data);
	void rxDataTimeout();
public slots:
	void sendCommand(int cmd, const QByteArray& data);
signals:
	void driverInfo(/*QFLog::Level*/int level, const QString &msg);
	void messageReady(const SIMessageData &msg);
	void rawDataReceived(const QByteArray &data);
public:
	SIDeviceDriver(QObject *parent = NULL);
	virtual ~SIDeviceDriver();
};

#endif // SIDEVICEDRIVER_H

