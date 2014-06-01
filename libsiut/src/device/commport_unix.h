
//
// Author: Frantisek Vacek <fanda.vacek@volny.cz>, (C) 2012
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef COMMPORT_UNIX_H
#define COMMPORT_UNIX_H

//#include <qglobal.h>

//#ifdef Q_OS_UNIX

#include "commport.h"

#include <sys/termios.h>

//! TODO: write class documentation.
class  CommPort_unix : public CommPort
{
protected:
	struct termios f_oldtio;
	int f_ttyFd;
public:
	virtual QByteArray read();
	virtual void write(const QByteArray &data);
	virtual int open(const QString &device, int baudrate, int data_bits, const QString& parity, bool two_stop_bits);
	virtual int close();
	virtual bool isOpen();
public:
	CommPort_unix(QObject *parent = NULL);
	virtual ~CommPort_unix();
};

//#endif // Q_OS_UNIX

#endif // COMMPORT_UNIX_H

