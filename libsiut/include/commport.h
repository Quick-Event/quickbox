
//
// Author: Frantisek Vacek <fanda.vacek@volny.cz>, (C) 2012
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef COMMPORT_H
#define COMMPORT_H

#include <QObject>

//! TODO: write class documentation.
class  CommPort : public QObject
{
	Q_OBJECT
protected:
protected:
	virtual void emitDriverInfo(int level, const QString &msg);
public:
	virtual void write(const QByteArray &data) = 0;
	virtual QByteArray read() = 0;
	/// @return 0 if no error
	virtual int open(const QString &device, int baudrate, int data_bits, const QString& parity, bool two_stop_bits) = 0;
	/// @return 0 if no error
	virtual int close() = 0;
	virtual bool isOpen() = 0;
signals:
	//void dataReceived(QByteArray data);
	void driverInfo(/*QFLog::Level*/int level, const QString &msg);
	//virtual void writeData(const QByteArray &data);
public:
	CommPort(QObject *parent = NULL);
	virtual ~CommPort();
};

#endif // COMMPORT_H

