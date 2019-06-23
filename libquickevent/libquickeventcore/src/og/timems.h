#ifndef QUICKEVENTCORE_OG_TIMEMS_H
#define QUICKEVENTCORE_OG_TIMEMS_H

#include "../quickeventcoreglobal.h"

#include <QChar>
#include <QMetaType>

class QString;

namespace quickevent {
namespace core {
namespace og {

class QUICKEVENTCORE_DECL_EXPORT TimeMsBase
{
public:
	TimeMsBase();
	TimeMsBase(int msec);
public:
	bool isValid() const {return m_isValid;}
	bool operator==(const TimeMsBase &o) const
	{
		if(!isValid())
			return !o.isValid();
		if(!o.isValid())
			return !isValid();
		return msec() == o.msec();
	}
	bool operator<(const TimeMsBase &o) const
	{
		if(!isValid())
			return o.isValid();
		if(!o.isValid())
			return false;
		return msec() < o.msec();
	}

	//QString toString(QChar sec_sep = QChar('.'), QChar msec_sep = QChar()) const;
	//static LapTimeMs fromString(const QString &time_str);
	int msec() const {return isValid()? m_msec: 0;}

	/// while time2 < time1 add 12 hours to time2 and return it
	static int fixTimeWrapAM(int time1_msec, int time2_msec);
	static int msecIntervalAM(int from_time_msec, int to_time_msec);

	//static void registerQVariantFunctions();
protected:
	int m_msec;
	bool m_isValid;
};

class QUICKEVENTCORE_DECL_EXPORT LapTimeMs : public TimeMsBase
{
	using Super = TimeMsBase;
public:
	static constexpr int UNREAL_TIME_MSEC = 9999 * 60 * 1000;
	static constexpr int NOT_FINISH_TIME_MSEC = UNREAL_TIME_MSEC;
	static constexpr int NOT_COMPETITING_TIME_MSEC = NOT_FINISH_TIME_MSEC - 1;
	static constexpr int DISQ_TIME_MSEC = NOT_COMPETITING_TIME_MSEC - 1;
	static constexpr int MAX_REAL_TIME_MSEC = DISQ_TIME_MSEC - 1;

	LapTimeMs();
	LapTimeMs(int msec);
public:
	QString toString(QChar sec_sep = QChar('.'), QChar msec_sep = QChar()) const;
	static LapTimeMs fromString(const QString &time_str);

	static void registerQVariantFunctions();
};

class QUICKEVENTCORE_DECL_EXPORT StpTimeMs : public TimeMsBase
{
	using Super = TimeMsBase;
public:
	StpTimeMs();
	StpTimeMs(int msec);
public:
	QString toString(QChar sec_sep = QChar('.'), QChar msec_sep = QChar()) const;
	static LapTimeMs fromString(const QString &time_str);

	static void registerQVariantFunctions();
};

}}}

Q_DECLARE_METATYPE(quickevent::core::og::LapTimeMs)
Q_DECLARE_METATYPE(quickevent::core::og::StpTimeMs)

#endif // QUICKEVENTCORE_OG_TIMEMS_H
