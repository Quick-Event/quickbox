#ifndef QUICKEVENT_OG_TIMEMS_H
#define QUICKEVENT_OG_TIMEMS_H

#include "../quickeventglobal.h"

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

	QString toString(bool including_msec = false) const;
	static TimeMs fromString(const QString &time_str);
	int msec() const {return m_msec;}
private:
	int m_msec;
	bool m_isValid;
};

}}

Q_DECLARE_METATYPE(quickevent::og::TimeMs)

#endif // QUICKEVENT_OG_TIMEMS_H
