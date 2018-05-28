#ifndef QUICKEVENT_OG_TIMEMS_H
#define QUICKEVENT_OG_TIMEMS_H

#include "../quickeventglobal.h"

#include <QChar>
#include <QMetaType>

class QString;

namespace quickevent {
namespace og {

class QUICKEVENT_DECL_EXPORT TimeMs
{
public:
	TimeMs();
	TimeMs(int msec);
public:
	bool isValid() const {return m_isValid;}
	bool operator==(const TimeMs &o) const
	{
		if(!isValid())
			return !o.isValid();
		if(!o.isValid())
			return !isValid();
		return msec() == o.msec();
	}
	bool operator<(const TimeMs &o) const
	{
		if(!isValid())
			return o.isValid();
		if(!o.isValid())
			return false;
		return msec() < o.msec();
	}

	QString toString(QChar sec_sep = QChar('.'), QChar msec_sep = QChar()) const;
	static TimeMs fromString(const QString &time_str);
	int msec() const {return isValid()? m_msec: 0;}

	/// while time2 < time1 add 12 hours to time2 and return it
	static int fixTimeWrapAM(int time1_msec, int time2_msec);
	static int msecIntervalAM(int from_time_msec, int to_time_msec);

	static void registerQVariantFunctions();
private:
	int m_msec;
	bool m_isValid;
};

}}

Q_DECLARE_METATYPE(quickevent::og::TimeMs)

#endif // QUICKEVENT_OG_TIMEMS_H
