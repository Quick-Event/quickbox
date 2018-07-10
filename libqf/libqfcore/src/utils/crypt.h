#ifndef QF_CORE_UTILS_CRYPT_H
#define QF_CORE_UTILS_CRYPT_H

#include "../core/coreglobal.h"

#include <limits>
#include <functional>

namespace qf {
namespace core {
namespace utils {

class QFCORE_DECL_EXPORT Crypt
{
public:
	typedef std::function< quint32 (quint32) > Generator;
public:
	Crypt(Generator gen = nullptr);
public:
	static Generator createGenerator(quint32 a, quint32 b, quint32 max_rand);

	/// any of function, functor or lambda can be set as a random number generator
	void setGenerator(Generator gen) {m_generator = gen;}

	/// pouziva muj vlastni, pomerne prustrelny kryptovaci mechanismus
	/// @return libovolny string zakrypti do stringu obsahujiciho znaky 0-9, A-Z, a-z
	/// pokud je to, co se crypti moc kratky, je to docpano vatou tak, aby zakrypteny string mel minimalne \a min_length znaku, muze mit o jeden vic
	Q_REQUIRED_RESULT QByteArray encrypt(const QByteArray &data, int min_length = 16) const;

	/// inverzni operace k funkci crypt()
	Q_REQUIRED_RESULT QByteArray decrypt(const QByteArray &data) const;
private:
	Q_REQUIRED_RESULT QByteArray decodeArray(const QByteArray &ba) const;
protected:
	Generator m_generator;
};

}}}

#endif // QF_CORE_UTILS_CRYPT_H
