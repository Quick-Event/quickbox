#include "crypt.h"

#include "../core/log.h"

#include <QTime>

using namespace qf::core::utils;

//===================================================================
//                                         Crypt
//===================================================================
/// http://www.math.utah.edu/~pa/Random/Random.html
Crypt::Crypt(Crypt::Generator gen)
	: m_generator(gen)
{
	if(m_generator == nullptr)
		m_generator = Crypt::createGenerator(16811, 7, 2147483647);
}

Crypt::Generator Crypt::createGenerator(quint32 a, quint32 b, quint32 max_rand)
{
	auto ret = [a, b, max_rand](quint32 val) -> quint32 {
		quint64 ret = val;
		ret *= a;
		ret += b;
		ret %= max_rand;
		//qfWarning() << '(' << a << '*' << val << '+' << b << ") %" << max_rand << "---->" << ret;
		return ret;
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
		quint8 b1 = b%10;
		b /= 10;
		buff[0] = b1 + '0';
		buff[1] = (b1 % 2)? b + 'A': b + 'a';
		ret.append(buff);
	}
	return ret;
}

QByteArray Crypt::encrypt(const QByteArray &data, int min_length) const
{
	QByteArray dest;

	/// nahodne se vybere hodnota, kterou se string zaxoruje a ta se ulozi na zacatek
	unsigned val = (unsigned)qrand();
	val += QTime::currentTime().msec();
	val %= 256;
	if(val == 0)
		val = 1;/// fix case vhen val == 0 and generator C == 0 also
	quint8 b = (quint8)val;
	dest += code_byte(b);

	/// a tou se to zaxoruje
	for(int i=0; i<data.count(); i++) {
		b = ((quint8)data[i]);
		if(b == 0)
			break;
		val = m_generator(val);
		b = b ^ (quint8)val;
		dest += code_byte(b);
	}
	quint8 bb = 0;
	while(dest.size() < min_length) {
		val = m_generator(val);
		b = bb ^ (quint8)val;
		dest += code_byte(b);
		bb = (quint8)qrand();
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
		b1 = b1 - '0';
		if(i < ba.size()) {
			b = ba[i++];
			b = (b1 % 2)? (b - 'A'): (b - 'a');
			b = 10 * b + b1;
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

QByteArray Crypt::decrypt(const QByteArray &data) const
{
	/// odstran vsechny bile znaky, v zakodovanem textu nemohou byt, muzou to byt ale zalomeni radku
	QByteArray ba = data.simplified();
	ba.replace(' ', "");
	ba = decodeArray(ba);
	///odstran \0 na konci, byly tam asi umele pridany
	int pos = 0;
	while(pos < ba.size()) {
		if(ba[pos] == '\0')
			break;
		pos++;
	}
	ba = ba.mid(0, pos);
	return ba;
}
