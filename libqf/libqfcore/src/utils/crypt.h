#ifndef QF_CORE_UTILS_CRYPT_H
#define QF_CORE_UTILS_CRYPT_H

#include "../core/coreglobal.h"

#include <limits>

namespace qf {
namespace core {
namespace utils {

class QFCORE_DECL_EXPORT Crypt
{
public:
	typedef std::function< unsigned (unsigned) > Generator;
public:
	Crypt(Generator gen = NULL);
public:
	static Generator createGenerator(unsigned a, unsigned b, unsigned max_rand);

	/// any of function, functor or lambda can be set as a random number generator
	void setGenerator(Generator gen) {m_generator = gen;}

	/// pouziva muj vlastni, pomerne prustrelny kryptovaci mechanismus
	/// @return libovolny string zakrypti do stringu obsahujiciho znaky 0-9, A-Z, a-z
	/// pokud je to, co se crypti moc kratky, je to docpano vatou tak, aby zakrypteny string mel minimalne \a min_length znaku, muze mit o jeden vic
	QByteArray encrypt(const QString &s, int min_length = 10) const;

	/// inverzni operace k funkci crypt()
	QString decrypt(const QByteArray &ba) const;
private:
	QByteArray decodeArray(const QByteArray &ba) const;
private:
	Generator m_generator;
};

}}}

#endif // QF_CORE_UTILS_CRYPT_H
