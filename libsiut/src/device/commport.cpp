//
// Author: Frantisek Vacek <fanda.vacek@volny.cz>, (C) 2012
//
// Copyright: See COPYING file that comes with this distribution
//

#include "commport.h"

#include <qf/core/log.h>

#include <QSerialPortInfo>

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

bool CommPort::openComm(const QString &_device, int _baudrate, int _data_bits, const QString &_parity_str, bool two_stop_bits)
{
	qfLogFuncFrame();
	close();
	QString device = _device;
	{
		qfDebug() << "Port enumeration";
		QList<QSerialPortInfo> port_list = QSerialPortInfo::availablePorts();
		QStringList sl;
		for(auto port : port_list) {
			if(device.isEmpty())
				device = port.systemLocation();
			sl << QString("%1 %2").arg(port.portName()).arg(port.systemLocation());
			qfDebug() << "\t" << port.portName();
		}
		emitCommInfo(qf::core::Log::Level::Info, tr("Available ports: %1").arg(sl.join(QStringLiteral(", "))));
	}
	setPortName(device);
	setBaudRate(_baudrate);
	setDataBitsAsInt(_data_bits);
	setParityAsString(_parity_str);
	setStopBits(two_stop_bits? QSerialPort::TwoStop: QSerialPort::OneStop);
	//f_commPort->setFlowControl(p.flowControl);
	emitCommInfo(qf::core::Log::Level::Debug, tr("Connecting to %1 - baudrate: %2, data bits: %3, parity: %4, stop bits: %5")
				   .arg(portName())
				   .arg(baudRate())
				   .arg(dataBits())
				   .arg(parity())
				   .arg(stopBits())
				   );
	bool ret = Super::open(QIODevice::ReadWrite);
	if(ret) {
		emit openChanged(true);
		emitCommInfo(qf::core::Log::Level::Info, tr("%1 connected OK").arg(device));
	}
	else {
		emit openChanged(false);
		emitCommInfo(qf::core::Log::Level::Error, tr("%1 connect ERROR: %2").arg(device).arg(errorString()));
	}
	return ret;
}

void CommPort::closeComm()
{
	if(isOpen()) {
		Super::close();
		emit openChanged(false);
		emitCommInfo(qf::core::Log::Level::Info, tr("%1 closed").arg(portName()));
	}
}

void CommPort::sendData(const QByteArray &data)
{
	qint64 n = write(data);
	if(n != data.size())
		qfError() << "send data error!";
}

void CommPort::emitCommInfo ( qf::core::Log::Level level, const QString& msg )
{
	//qfLog(level) << msg;
	switch (level) {
	case qf::core::Log::Level::Debug: qfDebug() << msg; break;
	case qf::core::Log::Level::Info: qfInfo() << msg; break;
	case qf::core::Log::Level::Warning: qfWarning() << msg; break;
	default: qfError() << msg; break;
	}
	emit commInfo(level, msg);
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
