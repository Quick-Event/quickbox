#include "crypt.h"

#include "../core/log.h"

#include <QTime>

using namespace qf::core::utils;

//===================================================================
//                                         Crypt
//===================================================================
/// http://www.math.utah.edu/~pa/Random/Random.html
Crypt::Crypt(Crypt::Generator gen)
{
	m_generator = gen;
	if(m_generator == NULL)
		m_generator = Crypt::createGenerator(16811, 7, 2147483647);
}

Crypt::Generator Crypt::createGenerator(unsigned a, unsigned b, unsigned max_rand)
{
	auto ret = [=](unsigned val) {
		return (a * val + b) % max_rand;
	};
	return ret;
}

static QByteArray code_byte(quint8 b)
{
	QByteArray ret;
	/// hodnoty, ktere nejsou pismena se ukladaji jako cislo
	/// format cisla je 4 bity cislo % 10 [0-9] + 4 bity cislo / 10 [A-Z]
	char buff[] = {0,0,0};
	if((b>='A' && b<='Z') || (b>='a' && b<='z')) {
		ret.append(b);
	}
	else {
		buff[0] = b%10 + '0';
		buff[1] = b/10 + 'A';
		ret.append(buff);
	}
	return ret;
}

QByteArray Crypt::encrypt(const QString &s, int min_length) const
{
	QByteArray dest;
	QByteArray src = s.toUtf8();

	/// nahodne se vybere hodnota, kterou se string zaxoruje a ta se ulozi na zacatek
	unsigned val = (unsigned)qrand();
	val += QTime::currentTime().msec();
	val %= 256;
	if(val == 0)
		val = 1;/// fix case vhen val == 0 and generator C == 0 also
	quint8 b = (quint8)val;
	dest += code_byte(b);

	/// a tou se to zaxoruje
	for(int i=0; i<src.count(); i++) {
		val = m_generator(val);
		b = ((quint8)src[i]);
		b = b ^ (quint8)val;
		dest += code_byte(b);
	}
	while(dest.size() < min_length) {
		val = m_generator(val);
		b = 0 ^ (quint8)val;
		dest += code_byte(b);
	}
	return dest;
}

static quint8 take_byte(const QByteArray &ba, int &i)
{
	quint8 b = ((quint8)ba[i++]);
	if((b>='A' && b<='Z') || (b>='a' && b<='z')) {
	}
	else {
		quint8 b1 = b;
		b = b1 - '0';
		if(i < ba.size()) {
			b1 = ba[i++];
			b +=  10 * (b1 - 'A');
		}
		else {
			qfError() << QF_FUNC_NAME << ": byte array corupted:" << ba.constData();
		}
	}
	return b;
}

QByteArray Crypt::decodeArray(const QByteArray &ba) const
{
	/// vyuziva toho, ze generator nahodnych cisel generuje pokazde stejnou sekvenci
	/// precte si seed ze zacatku \a ba a pak odxorovava nahodnymi cisly, jen to svisti
	qfLogFuncFrame() << "decoding:" << ba.constData();
	QByteArray ret;
	if(ba.isEmpty()) return ret;
	int i = 0;
	unsigned val = take_byte(ba, i);
	while(i<ba.count()) {
		val = m_generator(val);
		quint8 b = take_byte(ba, i);
		b = b ^ (quint8)val;
		ret.append(b);
	}
	return ret;
}

QString Crypt::decrypt(const QByteArray &_ba) const
{
	/// odstran vsechny bile znaky, v zakodovanem textu nemohou byt, muzou to byt ale zalomeni radku
	QByteArray ba = _ba.simplified();
	ba.replace(' ', "");
	ba = decodeArray(ba);
	///odstran \0 na konci, byly tam asi umele pridany
	int pos = ba.size();
	while(pos > 0) {
		pos--;
		if(ba[pos] == '\0') {
		}
		else {
			pos++;
			break;
		}
	}
	ba = ba.mid(0, pos);
	return QString::fromUtf8(ba);
}
