
//
// Author: Frantisek Vacek <fanda.vacek@volny.cz>, (C) 2012
//
// Copyright: See COPYING file that comes with this distribution
//

#include "commport.h"

#include <qf/core/log.h>

using namespace siut;

//=================================================
//             CommPort
//=================================================
CommPort::CommPort(QObject *parent)
: Super(parent)
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

void CommPort::setDataBitsAsInt(int data_bits)
{
	DataBits db;
	switch(data_bits) {
	case 5: db = Data5; break;
	case 6: db = Data6; break;
	case 7: db = Data7; break;
	case 8: db = Data8; break;
	default: db = UnknownDataBits; break;
	}
	setDataBits(db);
}

void CommPort::setParityAsString(const QString &parity_str)
{
	Parity parity = NoParity;
	if(parity_str.compare(QStringLiteral("odd"), Qt::CaseInsensitive)) parity = OddParity;
	else if(parity_str.compare(QStringLiteral("even"), Qt::CaseInsensitive)) parity = EvenParity;
	else if(parity_str.compare(QStringLiteral("space"), Qt::CaseInsensitive)) parity = SpaceParity;
	else if(parity_str.compare(QStringLiteral("mark"), Qt::CaseInsensitive)) parity = MarkParity;
	setParity(parity);
}
