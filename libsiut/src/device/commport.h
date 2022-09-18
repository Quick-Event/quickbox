
//
// Author: Frantisek Vacek <fanda.vacek@volny.cz>, (C) 2012
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef SIUT_COMMPORT_H
#define SIUT_COMMPORT_H

#include <siut/siutglobal.h>

#include <qf/core/log.h>

#include <QSerialPort>

namespace siut {

class SIUT_DECL_EXPORT CommPort : public QSerialPort
{
	Q_OBJECT
private:
	typedef QSerialPort Super;
public:
	CommPort(QObject *parent = nullptr);
	virtual ~CommPort();
public:
	Q_SIGNAL void openChanged(bool is_open);
	bool openComm(const QString &device, int baudrate, int data_bits, const QString& parity, bool two_stop_bits);
	void closeComm();
	/*
	QString errorString();
	QString portName();
	*/
	void sendData(const QByteArray &data);

	Q_SIGNAL void commInfo(NecroLog::Level level, const QString &msg);

	QString errorToUserHint() const;
protected:
	virtual void emitCommInfo(NecroLog::Level level, const QString &msg);

	void setDataBitsAsInt(int data_bits);
	void setParityAsString(const QString &parity_str);
};

}

#endif // SIUT_COMMPORT_H

