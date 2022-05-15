
//
// Author: Frantisek Vacek <fanda.vacek@volny.cz>, (C) 2011
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef SIUT_SIDEVICEDRIVER_H
#define SIUT_SIDEVICEDRIVER_H

#include "sitask.h"

#include <siut/simessagedata.h>

#include <siut/siutglobal.h>

#include <necrolog.h>

#include <QPointer>

class QTimer;

namespace siut {

class SIUT_DECL_EXPORT DeviceDriver : public QObject
{
	Q_OBJECT
private:
	typedef QObject Super;

public:
	DeviceDriver(QObject *parent = nullptr);
	virtual ~DeviceDriver();

	/// public to allow injection SI data from different sources
	/// like UDP socket
	virtual void processData(const QByteArray &data);

	void sendCommand(int cmd, const QByteArray& data);
	void setSiTask(SiTask *task);

	void sendACK();

	Q_SIGNAL void driverInfo(NecroLog::Level level, const QString &msg);
	//Q_SIGNAL void siMessageReceived(const SIMessageData &msg);
	//Q_SIGNAL void siDatagramReceived(const QByteArray &data);
	Q_SIGNAL void dataToSend(const QByteArray &data);
	Q_SIGNAL void siTaskFinished(int task_type, QVariant result);
protected:
	//virtual void onSiMessageReceived(const SIMessageData &msg);
	void processSIMessageData(const SIMessageData &msg_data);
	void emitDriverInfo(NecroLog::Level level, const QString &msg);
private:
	//void sendAck();
	//void abortMessage();
protected:
	QByteArray f_rxData;
	SIMessageData f_messageData;
	SiTask *m_currentTask = nullptr;
};

}

#endif // SIDEVICEDRIVER_H

