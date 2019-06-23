#include "timems.h"

#include <qf/core/log.h>

#include <QString>

namespace quickevent {
namespace core {
namespace og {

static QString n2str(int n, int width = 2)
{
	QString ret = QString::number(n);
	while (ret.length() < width)
		ret = '0' + ret;
	return ret;
};

static int take_n(const QString &str, int &ix)
{
	int ret = 0;
	for (; ix < str.length(); ++ix) {
		char c = str.at(ix).toLatin1();
		if(c >= '0' && c <= '9')
			ret += 10 * ret + (c - '0');
		else
			break;
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

//===================================================
// TimeMsBase
//===================================================

TimeMsBase::TimeMsBase()
	: m_msec(0), m_isValid(false)
{
}

TimeMsBase::TimeMsBase(int msec)
	: m_msec(msec), m_isValid(true)
{

}

int TimeMsBase::fixTimeWrapAM(int time1_msec, int time2_msec)
{
	constexpr int hr12ms = 12 * 60 * 60 * 1000;
	while(time2_msec < time1_msec)
		time2_msec += hr12ms;
	while(time1_msec <= time2_msec - hr12ms)
		time2_msec -= hr12ms;
	return time2_msec;
}

int TimeMsBase::msecIntervalAM(int from_time_msec, int to_time_msec)
{
	return fixTimeWrapAM(from_time_msec, to_time_msec) - from_time_msec;
}

//===================================================
// LapTimeMs
//===================================================
LapTimeMs::LapTimeMs()
	: Super()
{
}

LapTimeMs::LapTimeMs(int msec)
	: Super(msec)
{
}

QString LapTimeMs::toString(QChar sec_sep, QChar msec_sep) const
{
	if(!isValid())
		return QString();

	int msec = m_msec;
	bool is_neg = false;
	if(msec < 0) {
		msec = -msec;
		is_neg = true;
	}
	int ms = msec % 1000;
	int sec = (msec / 1000) % 60;
	int min = msec / (1000 * 60);
	QString ret = n2str(min);
	ret += sec_sep + n2str(sec);
	if(!msec_sep.isNull())
		ret += msec_sep + n2str(ms, 3);
	if(is_neg)
		ret = '-' + ret;
	return ret;
}

LapTimeMs LapTimeMs::fromString(const QString &time_str)
{
	if(time_str.isEmpty())
		return LapTimeMs();

	int msec = 0;
	int sec = 0;
	int min = 0;
	int ix1 = 0;

	int ix2 = time_str.indexOf('.', ix1);
	if(ix2 < 0)
		ix2 = time_str.indexOf(':', ix1);
	if(ix2 < 0)
		ix2 = time_str.indexOf('-', ix1);
	if(ix2 < 0)
		ix2 = time_str.indexOf(',', ix1);
	if(ix2 < 0)
		ix2 = time_str.length();
	min = str2int(time_str.mid(ix1, ix2));
	ix1 = ix2 + 1;

	ix2 = time_str.indexOf('/', ix1);
	if(ix2 < 0)
		ix2 = time_str.length();
	sec = str2int(time_str.mid(ix1, ix2));
	ix1 = ix2 + 1;

	msec = str2int(time_str.mid(ix1));

	return LapTimeMs(msec + (sec + (min * 60)) * 1000);
}

void LapTimeMs::registerQVariantFunctions()
{
	static bool registered = false;
	if(!registered) {
		registered = true;
		{
			bool ok = QMetaType::registerComparators<LapTimeMs>();
			if(!ok)
				qfError() << "Error registering comparators for quickevent::core::og::TimeMs!";
		}
		{
			bool ok = QMetaType::registerConverter<LapTimeMs, int>([](const LapTimeMs &t) -> int {return t.msec();});
			if(!ok)
				qfError() << "Error registering converter for quickevent::core::og::LapTimeMs!";
		}
	}
}

//===================================================
// StpTimeMs
//===================================================

StpTimeMs::StpTimeMs()
: Super()
{
}

StpTimeMs::StpTimeMs(int msec)
: Super(msec)
{
}

QString StpTimeMs::toString(QChar sec_sep, QChar msec_sep) const
{
	if(!isValid())
		return QString();

	int msec = m_msec;
	bool is_neg = false;
	if(msec < 0) {
		msec = -msec;
		is_neg = true;
	}
	int ms = msec % 1000;
	msec /= 1000;
	int sec = msec % 60;
	msec /= 60;
	int min = msec % 60;
	int hour = msec / 60;
	QString ret = n2str(hour);
	ret += sec_sep + n2str(min);
	ret += sec_sep + n2str(sec);
	if(!msec_sep.isNull())
		ret += msec_sep + n2str(ms, 3);
	if(is_neg)
		ret = '-' + ret;
	return ret;
}

LapTimeMs StpTimeMs::fromString(const QString &time_str)
{
	if(time_str.isEmpty())
		return LapTimeMs();

	int msec = 0;
	int sec = 0;
	int min = 0;
	int hour = 0;
	int ix1 = 0;

	int ix2 = time_str.indexOf('.', ix1);
	if(ix2 < 0)
		ix2 = time_str.indexOf(':', ix1);
	if(ix2 < 0)
		ix2 = time_str.indexOf('-', ix1);
	if(ix2 < 0)
		ix2 = time_str.indexOf(',', ix1);
	if(ix2 < 0)
		ix2 = time_str.length();
	min = str2int(time_str.mid(ix1, ix2));
	ix1 = ix2 + 1;

	ix2 = time_str.indexOf('/', ix1);
	if(ix2 < 0)
		ix2 = time_str.length();
	sec = str2int(time_str.mid(ix1, ix2));
	ix1 = ix2 + 1;

	msec = str2int(time_str.mid(ix1));

	return LapTimeMs(msec + (sec + (min * 60)) * 1000);
}

void StpTimeMs::registerQVariantFunctions()
{
	static bool registered = false;
	if(!registered) {
		registered = true;
		{
			bool ok = QMetaType::registerComparators<StpTimeMs>();
			if(!ok)
				qfError() << "Error registering comparators for quickevent::core::og::StpTimeMs!";
		}
		{
			bool ok = QMetaType::registerConverter<StpTimeMs, int>([](const StpTimeMs &t) -> int {return t.msec();});
			if(!ok)
				qfError() << "Error registering converter for quickevent::core::og::StpTimeMs!";
		}
	}
}

}}}
