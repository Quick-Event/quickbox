#ifndef QUICKEVENT_OGTIMEMS_H
#define QUICKEVENT_OGTIMEMS_H

#include "../quickeventglobal.h"

#include <QMetaType>

class QString;

class QUICKEVENT_DECL_EXPORT OGTimeMs
{
public:
	OGTimeMs(int msec = 0);
public:
	QString toString(bool including_msec = false) const;
	static OGTimeMs fromString(const QString &time_str);
	int msec() const {return m_msec;}
private:
	int m_msec;
};

Q_DECLARE_METATYPE(OGTimeMs)

#endif // QUICKEVENT_OGTIMEMS_H
