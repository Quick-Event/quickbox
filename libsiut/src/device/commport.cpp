
//
// Author: Frantisek Vacek <fanda.vacek@volny.cz>, (C) 2012
//
// Copyright: See COPYING file that comes with this distribution
//

#include "commport.h"

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

void CommPort::emitDriverInfo ( qf::core::Log::Level level, const QString& msg )
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
	if(parity_str.compare(QLatin1String("odd"), Qt::CaseInsensitive) == 0) parity = OddParity;
	else if(parity_str.compare(QLatin1String("even"), Qt::CaseInsensitive) == 0) parity = EvenParity;
	else if(parity_str.compare(QLatin1String("space"), Qt::CaseInsensitive) == 0) parity = SpaceParity;
	else if(parity_str.compare(QLatin1String("mark"), Qt::CaseInsensitive) == 0) parity = MarkParity;
	setParity(parity);
}
