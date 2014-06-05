
//
// Author: Frantisek Vacek <fanda.vacek@volny.cz>, (C) 2012
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef SIUT_COMMPORT_H
#define SIUT_COMMPORT_H

#include <QSerialPort>

namespace siut {

//! TODO: write class documentation.
class  CommPort : public QSerialPort
{
	Q_OBJECT
private:
	typedef QSerialPort Super;
public:
	CommPort(QObject *parent = NULL);
	virtual ~CommPort();
protected:
	virtual void emitDriverInfo(int level, const QString &msg);
signals:
	void driverInfo(int level, const QString &msg);
public:
	void setDataBitsAsInt(int data_bits);
	void setParityAsString(const QString &parity_str);
};

}

#endif // SIUT_COMMPORT_H

