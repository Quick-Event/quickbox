#ifndef QUICKEVENT_SI_SIID_H
#define QUICKEVENT_SI_SIID_H

#include "../quickeventglobal.h"

#include <QMetaType>

namespace quickevent {
namespace si {

class QUICKEVENT_DECL_EXPORT SiId
{
public:
	SiId(int val = 0) : m_value(val) {}

	SiId& operator=(int i) {m_value = i; return *this;}
	operator int () const {return m_value;}
	bool operator == (const SiId &o) const {return m_value == o.m_value;}
	bool operator < (const SiId &o) const {return m_value < o.m_value;}

	static void registerQVariantFunctions();
private:
	int m_value = 0;
};

} // namespace og
} // namespace quickevent

Q_DECLARE_METATYPE(quickevent::si::SiId)

#endif // QUICKEVENT_SI_SIID_H
