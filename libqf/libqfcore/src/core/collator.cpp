#include "collator.h"
#include "log.h"

#include <QString>

using namespace qf::core;
/*
Collator::Collator()
{
	*this = sharedNull();
}
*/
Collator::Collator(QLocale::Language lang)
{
	d = new Data();
	setLanguage(lang);
}

Collator::Collator(Collator::SharedDummyHelper)
{
	d = new Data();
}

const Collator &Collator::sharedNull()
{
	static Collator n = Collator(SharedDummyHelper());
	return n;
}

int Collator::compare(const QString &s1, const QString &s2) const
{
	QStringRef sr1(&s1);
	QStringRef sr2(&s2);
	return compare(sr1, sr2);
}

int Collator::compare(const QStringRef &s1, const QStringRef &s2) const
{
	int ret = 0;
	for(int i=0; i<s1.length() && i<s2.length(); i++) {
		QChar c1 = s1.at(i);
		QChar c2 = s2.at(i);
		int n1 = sortIndex(c1);
		int n2 = sortIndex(c2);
		ret = n1 - n2;
		if(ret < 0) {
			ret = -1;
			break;
		}
		if(ret > 0) {
			ret = 1;
			break;
		}
	}
	if(ret == 0) {
		if(s1.length() < s2.length())
			ret = -1;
		else if(s1.length() > s2.length())
			ret = 1;
	}
	return ret;
}

int Collator::sortIndex(QChar c) const
{
	QChar co = c;
	if(caseSensitivity() == Qt::CaseInsensitive) {
		co = co.toLower();
	}
	if(ignorePunctuation())
		co = removePunctuation(language(), co);
	int ret = sortCache().value(co, -1);
	if(ret < 0) {
		return co.unicode() % 256;
	}
	if(co.isLower())
		ret += sortCache().size();
	ret += 256;
	return ret;
}

QByteArray Collator::toAscii7(QLocale::Language lang, const QString &s, bool to_lower)
{
	QByteArray ret;
	ret.reserve(s.length());
	for(int i=0; i < s.length(); i++) {
		QChar c = s[i];
		bool is_upper = c.isUpper();
		c = removePunctuation(lang, c.toLower());
		if(is_upper && !to_lower)
			c = c.toUpper();
		ret.append(c);
	}
	return ret;
}

static const char all_chars_cs[] = "aáäbcčdďeéěëfghiíïjklĺľmnňoóöpqrřsštťuůúüvwxyýzž";
static const char bt7_chars[] = "aaabccddeeeefghiiijklllmnnooopqrrssttuuuuvwxyyzz";

QChar Collator::removePunctuation(QLocale::Language lang, QChar c)
{
	Q_UNUSED(lang)
	static QHash<QChar, QChar> punct_to_bt7;
	if(punct_to_bt7.isEmpty()) {
		const QString all_str = QString::fromUtf8(all_chars_cs);
		const QString bt7_str = QString::fromUtf8(bt7_chars);
		Q_ASSERT(all_str.length() == bt7_str.length());
		for(int i=0; i<all_str.length(); i++)
			punct_to_bt7[all_str[i]] = bt7_str[i];
		for(int i=0; i<all_str.length(); i++)
			punct_to_bt7[all_str[i].toUpper()] = bt7_str[i].toUpper();
		//for(auto k : punct_to_bt7.keys())
		//	qfInfo() << k << "->" << punct_to_bt7.value(k);
	}
	return punct_to_bt7.value(c, c);
}

QHash<QChar, int> Collator::sortCache()
{
	static QHash<QChar, int> ret;
	if(ret.isEmpty()) {
		static const QString chars = QString::fromUtf8(all_chars_cs);
		for(int i=0; i<chars.length(); i++)
			ret[chars[i]] = i;
	}
	return ret;
}
