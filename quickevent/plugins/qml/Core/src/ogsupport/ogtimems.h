#ifndef OGTIMEMS_H
#define OGTIMEMS_H

#include <QMetaType>

class QString;

class OGTimeMs
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

#endif // OGTIMEMS_H
