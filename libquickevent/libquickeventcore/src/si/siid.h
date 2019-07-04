#ifndef QUICKEVENTCORE_SI_SIID_H
#define QUICKEVENTCORE_SI_SIID_H

#include "../quickeventcoreglobal.h"

#include <QMetaType>

namespace quickevent {
namespace core {
namespace si {

class QUICKEVENTCORE_DECL_EXPORT SiId
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

}}}

Q_DECLARE_METATYPE(quickevent::core::si::SiId)

#endif // QUICKEVENTCORE_SI_SIID_H
