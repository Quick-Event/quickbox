#ifndef QUICKEVENT_OG_SIID_H
#define QUICKEVENT_OG_SIID_H

#include <QMetaType>

namespace quickevent {
namespace og {

class SiId
{
public:
	SiId(int val = 0) : m_value(val) {}

	SiId& operator=(int i) {m_value = i; return *this;}
	operator int () const {return m_value;}
private:
	int m_value = 0;
};

} // namespace og
} // namespace quickevent

Q_DECLARE_METATYPE(quickevent::og::SiId)

#endif // QUICKEVENT_OG_SIID_H
