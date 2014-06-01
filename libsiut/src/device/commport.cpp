
//
// Author: Frantisek Vacek <fanda.vacek@volny.cz>, (C) 2012
//
// Copyright: See COPYING file that comes with this distribution
//

#include "commport.h"

#include <qf/core/logcust.h>

//=================================================
//             CommPort
//=================================================
CommPort::CommPort(QObject *parent)
: QObject(parent)
{
}

CommPort::~CommPort()
{
}

void CommPort::emitDriverInfo ( int level, const QString& msg )
{
	qfLog(level) << msg;
	emit driverInfo(level, msg);
}
