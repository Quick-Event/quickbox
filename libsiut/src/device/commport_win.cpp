
//
// Author: Frantisek Vacek <fanda.vacek@volny.cz>, (C) 2012
//
// Copyright: See COPYING file that comes with this distribution
//
//#ifdef Q_OS_WIN

#include "commport_win.h"

#include <qflogcust.h>

//=================================================
//             CommPort_win
//=================================================
CommPort_win::CommPort_win(QObject *parent)
: CommPort(parent)
{
	f_handle = INVALID_HANDLE_VALUE;
}

CommPort_win::~CommPort_win()
{
}

int CommPort_win::open(const QString& device, int _baud_rate, int data_bits, const QString& parity, bool two_stop_bits)
{
	int ret = -1;

	do {
		f_handle = CreateFileA(device.toStdString().c_str(),
					GENERIC_READ|GENERIC_WRITE,
					0,                          /* no share  */
					NULL,                       /* no security */
					OPEN_EXISTING,
					0,                          /* no threads */
					NULL);                      /* no templates */

		if(f_handle == INVALID_HANDLE_VALUE) {
			qfError() << "Unable to open comport:" << device;
			break;
		}

		int baud_rate = _baud_rate;
		switch(_baud_rate)
		{
			case CBR_110:
			case CBR_300:
			case CBR_600:
			case CBR_1200:
			case CBR_2400:
			case CBR_4800:
			case CBR_9600:
			case CBR_14400:
			case CBR_19200:
			case CBR_38400:
			case CBR_57600:
			case CBR_115200:
			case CBR_128000:
			case CBR_256000: break;
			default: baud_rate = 0; break;
		}
		if(baud_rate == 0) {
			qfError() << "invalid baudrate:" << _baud_rate;
			break;
		}

		//QString cfg_str = "baud=%1 data=%2 parity=%3 stop=%4";
		//cfg_str = cfg_str.arg(baud_rate).arg(data_bits);

		DCB port_settings;
		memset(&port_settings, 0, sizeof(port_settings));  /* clear the new struct  */
		port_settings.DCBlength = sizeof(port_settings);
		/*
		if(!BuildCommDCBA(cfg_str.toStdString().c_str(), &port_settings)) {
			qfError() << "unable to set comport dcb settings";
			CloseHandle(f_handle);
			ret = -1;
		}
		*/
		port_settings.fBinary=TRUE;
		port_settings.fInX=FALSE;
		port_settings.fOutX=FALSE;
		port_settings.fAbortOnError=FALSE;
		port_settings.fNull=FALSE;
		port_settings.BaudRate = baud_rate;
		port_settings.ByteSize = data_bits;
		if(parity.compare("Mark", Qt::CaseInsensitive) == 0) port_settings.Parity = MARKPARITY;
		else if(parity.compare("Space", Qt::CaseInsensitive) == 0) port_settings.Parity = SPACEPARITY;
		else if(parity.compare("Odd", Qt::CaseInsensitive) == 0) port_settings.Parity = ODDPARITY;
		else if(parity.compare("Even", Qt::CaseInsensitive) == 0) port_settings.Parity = EVENPARITY;
		else port_settings.Parity = NOPARITY;
		port_settings.fParity = (port_settings.Parity == NOPARITY)? false: true;
		port_settings.StopBits = two_stop_bits? TWOSTOPBITS: ONESTOPBIT;

		if(!SetCommState(f_handle, &port_settings)) {
			qfError() << "unable to set comport cfg settings. Error code:" << GetLastError();
			break;
		}

		//setStopBits(Settings.StopBits);
		//setFlowControl(Settings.FlowControl);
		//setTimeout(Settings.Timeout_Millisec);

		COMMTIMEOUTS cptimeouts;

		cptimeouts.ReadIntervalTimeout         = 3;//MAXDWORD;
		cptimeouts.ReadTotalTimeoutMultiplier  = 3;
		cptimeouts.ReadTotalTimeoutConstant    = 2;
		cptimeouts.WriteTotalTimeoutMultiplier = 3;
		cptimeouts.WriteTotalTimeoutConstant   = 2;

		if(!SetCommTimeouts(f_handle, &cptimeouts)) {
			qfError() << "unable to set comport time-out settings";
			break;
		}

		ret = 0;
	} while(false);
	if(ret == 0) {
		emitDriverInfo(QFLog::LOG_INF, tr("Open device '%1' OK.").arg(device));
	}
	else {
		close();
	}
	return ret;
}

int CommPort_win::close()
{
	int ret = 0;
	if(f_handle != INVALID_HANDLE_VALUE) CloseHandle(f_handle);
	f_handle = INVALID_HANDLE_VALUE;
	emitDriverInfo(QFLog::LOG_INF, tr("Comm device closed."));
	return ret;
}

bool CommPort_win::isOpen()
{
	qfInfo() << "is open:" << (f_handle != INVALID_HANDLE_VALUE) << "handle:" << f_handle;
	return (f_handle != INVALID_HANDLE_VALUE);
}

#define BUFF_SIZE 4096
QByteArray CommPort_win::read()
{
	char buff[BUFF_SIZE];
	QByteArray ba;
	DWORD bytes_read = 0;
	do {
		qfInfo() << "READ blocking ...";
		bool ok = ReadFile(f_handle, (void*)buff, (DWORD)BUFF_SIZE, &bytes_read, NULL);
		qfInfo() << "return:" << ok << "bytes:" << bytes_read;
		if(!ok) {
			emitDriverInfo(QFLog::LOG_ERR, tr("read error: %1 '%2'").arg(bytes_read).arg(GetLastError()));
			//qfError() << "read error: " << bytes_read << strerror(errno);
			break;
		}
		/// if the device "disappeared", e.g. from USB, we get a read event for 0 bytes
		else if(bytes_read == 0) {
			//close();
			break;
		}

		ba.append(buff, bytes_read);
	} while(bytes_read == BUFF_SIZE);
	return ba;
}

void CommPort_win::write(const QByteArray& data)
{
	qfLogFuncFrame();
	if(isOpen()) {
		bool ok = WriteFile(f_handle, (void*)data.constData(), (DWORD)data.length(), NULL, NULL);
		if(!ok) {
			emitDriverInfo(QFLog::LOG_ERR, tr("write error: %1").arg(GetLastError()));
			//qfError() << "read error: " << bytes_read << strerror(errno);
		}
	}
	else {
		qfWarning() << trUtf8("Writing to not open COM.");
	}
}

//#endif // Q_OS_WIN
