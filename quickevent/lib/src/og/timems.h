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
	TimeMs(int msec = 0);
public:
	QString toString(bool including_msec = false) const;
	static TimeMs fromString(const QString &time_str);
	int msec() const {return m_msec;}
private:
	int m_msec;
};

}}

Q_DECLARE_METATYPE(quickevent::og::TimeMs)

#endif // QUICKEVENT_OG_TIMEMS_H
