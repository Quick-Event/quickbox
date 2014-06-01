
//
// Author: Frantisek Vacek <fanda.vacek@volny.cz>, (C) 2012
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef COMMPORT_WIN_H
#define COMMPORT_WIN_H

//#include <qglobal.h>

//#ifdef Q_OS_WIN

#include "commport.h"

#include <windows.h>

//! TODO: write class documentation.
class  CommPort_win : public CommPort
{
protected:
	HANDLE f_handle;
public:
	virtual QByteArray read();
	virtual void write(const QByteArray &data);
	virtual int open(const QString &device, int baudrate, int data_bits, const QString& parity, bool two_stop_bits);
	virtual int close();
	virtual bool isOpen();
public:
	CommPort_win(QObject *parent = NULL);
	virtual ~CommPort_win();
};

//#endif // Q_OS_WIN

#endif // COMMPORT_WIN_H

