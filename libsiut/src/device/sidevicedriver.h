
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

#include <QPointer>

class QTimer;

namespace siut {

class SIUT_DECL_EXPORT SiTask : public QObject
{
	Q_OBJECT
private:
	typedef QObject Super;
public:
	SiTask(QObject *parent = nullptr);
	~SiTask() override;

	Q_SIGNAL void sigSendCommand(int cmd, const QByteArray &data);
	virtual void onSiMessageReceived(const QByteArray &msg) = 0;
	virtual void start() {deleteLater();}
	virtual void finishAndDestroy(bool ok, QVariant result);
	void abort() {finishAndDestroy(false, QVariant());}
	Q_SIGNAL void aboutToFinish();
	Q_SIGNAL void finished(bool ok, QVariant result);
protected:
	//void restartRxTimer();
	void sendCommand(int cmd, const QByteArray &data);
protected:
	QTimer *m_rxTimer;
};

class SIUT_DECL_EXPORT DeviceDriver : public QObject
{
	Q_OBJECT
private:
	typedef QObject Super;
public:
	enum class SiCommand {
		Invalid=0,
		//SICardDetectedOrRemoved='F', /// next byte shoul be 'I' (detect) or 'O' (removed)
		GetSystemData=0x83,
		SICard5Detected=0xE5,
		SICard6DetectedExt=0xE6,
		SICard8AndHigherDetectedExt=0xE8,
		SICardRemoved=0xE7,
		GetSICard5=0xB1,
		GetSICard6=0xE1,
		GetSICard8=0xEF,
		//GetPunch2=0x53, /// autosend only (ie. punch)
		SetDirectRemoteMode=0xF0,
		//TimeSend=0x54, /// autosend only (ie. trigger data)
		TransmitRecord=0xD3, /// autosend only (transmit punch or trigger data)

		DriverInfo=0x1000 /// Driver info (SI commands are only 8 bit long)
	};
	enum class SiMessageType {Invalid=0, CardEvent, CardReadOut, Punch, DriverInfo, Other};
	//enum ProcessRxDataStatus {StatusUnknown, StatusMessageOk, StatusMessageError};
public:
	DeviceDriver(QObject *parent = nullptr);
	virtual ~DeviceDriver();

	/// public to allow injection SI data from different sources
	/// like UDP socket
	virtual void processData(const QByteArray &data);

	void sendCommand(int cmd, const QByteArray& data);
	void setSiTask(SiTask *cmd);

	//Q_SIGNAL void commOpenChanged(bool is_open);
	Q_SIGNAL void driverInfo(qf::core::Log::Level level, const QString &msg);
	//Q_SIGNAL void siMessageReceived(const SIMessageData &msg);
	Q_SIGNAL void siDatagramReceived(const QByteArray &data);
	Q_SIGNAL void dataToSend(const QByteArray &data);
protected:
	virtual void onSiMessageReceived(const SIMessageData &msg);
	void processReceivedSiDatagram(const QByteArray &msg_data);
	//void processRxData();
	void emitDriverInfo(qf::core::Log::Level level, const QString &msg);
protected slots:
	//void onCommReadyRead();
	//void rxDataTimeout();
private:
	void sendAck();
	//void abortMessage();
protected:
	//QSocketNotifier *f_socketNotifier;
	//CommPort *f_commPort;
	QByteArray f_rxData;
	//QTimer *f_rxTimer;
	//ProcessRxDataStatus f_status = StatusUnknown;
	//int f_packetReceivedCount = 0;
	//int f_packetToFinishCount = 0;
	SIMessageData f_messageData;
	QPointer<SiTask> m_taskInProcess;
};

}

#endif // SIDEVICEDRIVER_H

