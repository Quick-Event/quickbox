#include "ogtimems.h"

#include <qf/core/log.h>

#include <QString>

OGTimeMs::OGTimeMs(int msec)
	: m_msec(msec)
{

}

QString OGTimeMs::toString(bool including_msec) const
{
	int msec = m_msec % 1000;
	int sec = (m_msec / 1000) % 60;
	int min = m_msec / (1000 * 60);
	QString ret = QString::number(min) + ':';
	if(sec < 10)
		ret += '0';
	ret += QString::number(sec);
	if(including_msec || msec > 0) {
		if(msec < 100)
			ret += '0';
		if(msec < 10)
			ret += '0';
		ret += '.';
		ret += QString::number(msec);
	}
	return ret;
}

static int str2int(const QString &str)
{
	int ret = 0;
	QString s = str.trimmed();
	if(!s.isEmpty()) {
		bool ok;
		ret = s.toInt(&ok);
		if(!ok)
			qfWarning() << "Invalid OGTime string" << str;
	}
	return ret;
}

OGTimeMs OGTimeMs::fromString(const QString &time_str)
{
	int msec = 0;
	int sec = 0;
	int min = 0;
	int ix1 = 0;

	int ix2 = time_str.indexOf(':', ix1);
	if(ix2 < 0)
		ix2 = time_str.length();
	min = str2int(time_str.mid(ix1, ix2));
	ix1 = ix2 + 1;

	ix2 = time_str.indexOf('.', ix1);
	if(ix2 < 0)
		ix2 = time_str.length();
	sec = str2int(time_str.mid(ix1, ix2));
	ix1 = ix2 + 1;

	msec = str2int(time_str.mid(ix1));

	return OGTimeMs(msec + (sec + (min * 60)) * 1000);
}

