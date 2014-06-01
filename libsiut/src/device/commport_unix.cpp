
//
// Author: Frantisek Vacek <fanda.vacek@volny.cz>, (C) 2012
//
// Copyright: See COPYING file that comes with this distribution
//

//#ifdef Q_OS_UNIX

#include "commport_unix.h"

#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include <qf/core/logcust.h>

//=================================================
//             CommPort_unix
//=================================================
CommPort_unix::CommPort_unix(QObject *parent)
: CommPort(parent)
{
	f_ttyFd = -1;
}

CommPort_unix::~CommPort_unix()
{
}

bool CommPort_unix::isOpen()
{
	return f_ttyFd >= 0;
}

int CommPort_unix::open(const QString& device, int baud_rate, int data_bits, const QString& parity, bool two_stop_bits)
{
	bool ret = -1;
	//f_rxData.clear();
	f_ttyFd = -1;
	/// code from CuteCom
	#if 0
	bool software_handshake = false;
	bool hardware_handshake = false;
	#endif
	do {
		QString dev = device;

		int flags = O_RDWR; //flags=O_WRONLY; //flags=O_RDONLY;

		//f_ttyFd = ::open(dev.toLatin1().constData(), flags | O_NONBLOCK);
		f_ttyFd = ::open(dev.toLatin1().constData(), flags);
		if(f_ttyFd < 0) {
			emitDriverInfo(qf::core::Log::LOG_ERR, tr("Could not open %1").arg(dev));
			break;
		}

		/// flushing is to be done after opening. This prevents first read and write to be spam'ish.
		tcflush(f_ttyFd, TCIOFLUSH);

		{
			/// save terminal settings
			//int n = fcntl(f_ttyFd, F_GETFL, 0);
			//fcntl(f_ttyFd, F_SETFL, n & ~O_NONBLOCK);
			if (tcgetattr(f_ttyFd, &f_oldtio) != 0) {
				emitDriverInfo(qf::core::Log::LOG_ERR, tr("tcgetattr() failed"));
				break;
			}
		}
		{
			/// setup terminal
			struct termios newtio;
			//   memset(&newtio, 0, sizeof(newtio));
			if(tcgetattr(f_ttyFd, &newtio)!=0) {
				emitDriverInfo(qf::core::Log::LOG_ERR, tr("tcgetattr() failed"));
			}

			speed_t _baud=0;
			switch (baud_rate)
			{
				#ifdef B0
				case      0: _baud=B0;     break;
				#endif
				#ifdef B50
				case     50: _baud=B50;    break;
				#endif
				#ifdef B75
				case     75: _baud=B75;    break;
				#endif
				#ifdef B110
				case    110: _baud=B110;   break;
				#endif
				#ifdef B134
				case    134: _baud=B134;   break;
				#endif
				#ifdef B150
				case    150: _baud=B150;   break;
				#endif
				#ifdef B200
				case    200: _baud=B200;   break;
				#endif
				#ifdef B300
				case    300: _baud=B300;   break;
				#endif
				#ifdef B600
				case    600: _baud=B600;   break;
				#endif
				#ifdef B1200
				case   1200: _baud=B1200;  break;
				#endif
				#ifdef B1800
				case   1800: _baud=B1800;  break;
				#endif
				#ifdef B2400
				case   2400: _baud=B2400;  break;
				#endif
				#ifdef B4800
				case   4800: _baud=B4800;  break;
				#endif
				#ifdef B7200
				case   7200: _baud=B7200;  break;
				#endif
				#ifdef B9600
				case   9600: _baud=B9600;  break;
				#endif
				#ifdef B14400
				case  14400: _baud=B14400; break;
				#endif
				#ifdef B19200
				case  19200: _baud=B19200; break;
				#endif
				#ifdef B28800
				case  28800: _baud=B28800; break;
				#endif
				#ifdef B38400
				case  38400: _baud=B38400; break;
				#endif
				#ifdef B57600
				case  57600: _baud=B57600; break;
				#endif
				#ifdef B76800
				case  76800: _baud=B76800; break;
				#endif
				#ifdef B115200
				case 115200: _baud=B115200; break;
				#endif
				#ifdef B128000
				case 128000: _baud=B128000; break;
				#endif
				#ifdef B230400
				case 230400: _baud=B230400; break;
				#endif
				#ifdef B460800
				case 460800: _baud=B460800; break;
				#endif
				#ifdef B576000
				case 576000: _baud=B576000; break;
				#endif
				#ifdef B921600
				case 921600: _baud=B921600; break;
				#endif
				default:
					emitDriverInfo(qf::core::Log::LOG_ERR, tr("Unsupported baud rate %1").arg(baud_rate));
					break;
			}
			if(_baud == 0) break;

			cfsetospeed(&newtio, (speed_t)_baud);
			cfsetispeed(&newtio, (speed_t)_baud);

			/* We generate mark and space parity ourself. */
			if (data_bits == 7 && ((parity.compare("Mark", Qt::CaseInsensitive) == 0) || (parity.compare("Space", Qt::CaseInsensitive) == 0))) {
				data_bits = 8;
			}
			switch (data_bits)
			{
				case 5:
					newtio.c_cflag = (newtio.c_cflag & ~CSIZE) | CS5;
					break;
				case 6:
					newtio.c_cflag = (newtio.c_cflag & ~CSIZE) | CS6;
					break;
				case 7:
					newtio.c_cflag = (newtio.c_cflag & ~CSIZE) | CS7;
					break;
				case 8:
				default:
					newtio.c_cflag = (newtio.c_cflag & ~CSIZE) | CS8;
					break;
			}
			newtio.c_cflag |= CLOCAL | CREAD;

			//parity
			newtio.c_cflag &= ~(PARENB | PARODD);
			if (parity.compare("Even", Qt::CaseInsensitive) == 0) {
				newtio.c_cflag |= PARENB;
			}
			else if (parity.compare("Odd", Qt::CaseInsensitive) == 0) {
				newtio.c_cflag |= (PARENB | PARODD);
			}

			//hardware handshake
			/*   if (hardwareHandshake)
			 *      newtio.c_cflag |= CRTSCTS;
			 *   else
			 *      newtio.c_cflag &= ~CRTSCTS;*/
			newtio.c_cflag &= ~CRTSCTS;

			//stopbits
			if (two_stop_bits) {
				newtio.c_cflag |= CSTOPB;
			}
			else {
				newtio.c_cflag &= ~CSTOPB;
			}

			//   newtio.c_iflag=IGNPAR | IGNBRK;
			newtio.c_iflag=IGNBRK;
			//   newtio.c_iflag=IGNPAR;
			#if 0
			//software handshake
			if (software_handshake) {
				newtio.c_iflag |= IXON | IXOFF;
		}
		else {
			newtio.c_iflag &= ~(IXON|IXOFF|IXANY);
		}
		#endif
		newtio.c_lflag=0;
		newtio.c_oflag=0;

		newtio.c_cc[VTIME]=1;
		newtio.c_cc[VMIN]=60;

		//   tcflush(m_fd, TCIFLUSH);
		if (tcsetattr(f_ttyFd, TCSANOW, &newtio)!=0) {
			emitDriverInfo(qf::core::Log::LOG_ERR, tr("tcsetattr() failed"));
			break;
		}
		#if 0
		int mcs=0;
		ioctl(fd, TIOCMGET, &mcs);
			mcs |= TIOCM_RTS;
			ioctl(fd, TIOCMSET, &mcs);

			if (tcgetattr(fd, &newtio)!=0) {
				qfError() << "tcgetattr() failed";
				break;
		}

		//hardware handshake
		if(hardware_handshake) {
				newtio.c_cflag |= CRTSCTS;
		}
		else {
			newtio.c_cflag &= ~CRTSCTS;
		}
		/*  if (on)
		 *     newtio.c_cflag |= CRTSCTS;
		 *  else
		 *     newtio.c_cflag &= ~CRTSCTS;*/
		if(tcsetattr(fd, TCSANOW, &newtio)!=0) {
				qfError() << "tcsetattr() failed";
				break;
		}
		#endif
		}
		ret = 0;
	} while(false);
	if(ret == 0) {
		emitDriverInfo(qf::core::Log::LOG_INF, tr("Open device '%1' OK.").arg(device));
	}
	return ret;
}

int CommPort_unix::close()
{
	qfLogFuncFrame();
	int ret = 0;
	if(f_ttyFd > -1) {
		tcsetattr(f_ttyFd, TCSANOW, &f_oldtio);
		ret = ::close(f_ttyFd);
		qfInfo() << "close TTY:" << ret;
	}
	f_ttyFd = -1;
	emitDriverInfo(qf::core::Log::LOG_INF, tr("Comm device closed."));
	return ret;
}

#define BUFF_SIZE 1024

QByteArray CommPort_unix::read()
{
	char buff[BUFF_SIZE];
	QByteArray ba;
	int bytes_read = 0;
	do {
		bytes_read = ::read(f_ttyFd, buff, BUFF_SIZE);

		if(bytes_read < 0) {
			if(bytes_read == EAGAIN) {
				/// there are no more bytes to read, strange situation because read buffer is large enough for SI station
				break;
			}
			else {
				emitDriverInfo(qf::core::Log::LOG_ERR, tr("read error: %1 '%2'").arg(bytes_read).arg(strerror(errno)));
				//qfError() << "read error: " << bytes_read << strerror(errno);
				break;
			}
		}
		/// if the device "disappeared", e.g. from USB, we get a read event for 0 bytes
		else if(bytes_read == 0) {
			close();
			break;
		}

		ba.append(buff, bytes_read);
	} while(bytes_read == BUFF_SIZE);
	return ba;
}

void CommPort_unix::write(const QByteArray& ba)
{
	qfLogFuncFrame();
	if(isOpen()) {
		::write(f_ttyFd, ba.constData(), ba.length());
	}
	else {
		qfWarning() << trUtf8("Writing to not open fd.");
	}
}

//#endif // Q_OS_UNIX
