#include "string.h"

#include <QStringList>

using namespace qf::core;

qf::core::String qf::core::String::slice(int start, int end) const
{
	int l = length();
	if(start < 0) start += l;
	if(end < 0) end += l;
	if(start < 0) start = 0;
	if(end < 0) end = 0;
	if(end > l) end = l;
	if(start > end) start = end;
	//qfTrash() << "\tstart:" << start << "end:" << end;
	//qfTrash() << "\tthis:" << *this;
	return mid(start, end - start);
}


int String::pos(QChar what_char, QChar quote) const
{
	String s = *this;
	bool in_quotes = false;
	for(int i=0; i<length(); i++) {
		if(!quote.isNull() && s[i] == quote) {
			in_quotes = !in_quotes;
		}
		if(!in_quotes) {
			if(s[i] == what_char) return i;
		}
	}
	return -1;
}

QChar String::value(int ix) const
{
	if(ix < 0) ix += length();
	if(ix < 0 || ix >= length())
		return QChar();
	return QString::at(ix);
}

QStringList String::splitAndTrim(QChar sep, QChar quote, bool trim_parts, QString::SplitBehavior keep_empty_parts) const
{
	QStringList ret;
	String fs = *this, fs2;
	bool first_scan = true;
	while(!fs.isEmpty()) {
		if(first_scan)
			first_scan = false;
		else
			fs = fs.slice(1); // remove separator
		int ix = fs.pos(sep, quote);
		if(ix >= 0) {
			fs2 = fs.slice(0, ix);
			fs = fs.slice(ix);
		}
		else {
			fs2 = fs;
			fs = QString();
		}
		if(trim_parts) {
			fs2 = fs2.trimmed();
			if(fs2.value(0) == quote && fs2.value(-1) == quote)
				fs2 = fs2.slice(1, -1);
		}
		if(!fs2.isEmpty())
			ret.append(fs2);
		else if(keep_empty_parts == KeepEmptyParts)
			ret.append(fs2);
	}
	return ret;
}

int String::indexOfMatchingBracket(char opening_bracket, char closing_bracket, char quote) const
{
	int opens = 0;
	bool was_open = false;
	bool in_quotes = false;
	String s = *this;
	int i;
	QChar prev_c;
	for(i=0; i<length(); i++) {
		QChar c = s[i];
		if(prev_c != '\\') {
			if(quote && c == quote)
				in_quotes = !in_quotes;
			if(!in_quotes) {
				if(c == opening_bracket) {
					opens++;
					was_open = true;
				}
				else if(c == closing_bracket)
					opens--;
				if(was_open && opens==0) {
					return i;
				}
			}
		}
		prev_c = c;
	}
	if(opens != 0)
		return -1;
	return i;
}

QStringList String::splitBracketed(char sep, char opening_bracket, char closing_bracket, char quote, bool trim_parts, QString::SplitBehavior keep_empty_parts) const
{
	QStringList ret;
	String fs = this->trimmed();
	if(fs.value(0) == opening_bracket) {
		int ix = fs.indexOfMatchingBracket(opening_bracket, closing_bracket, quote);
		if(ix == fs.length() - 1)
			fs = fs.slice(1, -1);
	}
	while(!fs.isEmpty()) {
		int opens = 0;
		bool in_quotes = false;
		int i;
		QChar prev_c;
		for(i=0; i<fs.length(); i++) {
			QChar c = fs[i];
			if(prev_c != '\\') {
				if(quote && c == quote)
					in_quotes = !in_quotes;
				if(!in_quotes) {
					if(c == opening_bracket)
						opens++;
					else if(c == closing_bracket)
						opens--;
					if(opens == 0 && c == sep) {
						break;
					}
				}
			}
			prev_c = c;
		}
		String s = fs.slice(0, i);
		fs = fs.slice(i+1); /// skip separator
		if(trim_parts == TrimParts) {
			s = s.trimmed();
			if(s[0] == opening_bracket && s[-1] == closing_bracket) {
				s = s.slice(1, -1);
				s = s.trimmed();
			}
		}
		if(keep_empty_parts != KeepEmptyParts) {
			if(!s.isEmpty())
				ret << s;
		}
		else {
			ret << s;
		}
	}
	return ret;
}

bool String::toBool() const
{
	if(isEmpty())
		return false;
	if(!compare(QLatin1String("N"), Qt::CaseInsensitive))
		return false;
	if(!compare(QLatin1String("F"), Qt::CaseInsensitive))
		return false;
	if(!compare(QLatin1String("0"), Qt::CaseInsensitive))
		return false;
	if(!compare(QLatin1String("false"), Qt::CaseInsensitive))
		return false;
	return true;
}

QString String::number(double d, const QString &_format)
{
	String format = _format;
	QString ret;
	if(format.value(0) == 'N') {
		bool put_dec_point = true;
		bool negative = (d < 0);
		if(negative)
			d = -d;
		format = format.slice(2, -1).trimmed();
		int pos = format.indexOf(',');
		int thousands_places, decimal_places;
		if(pos >= 0) {
			thousands_places = format.slice(0, pos).toInt();
			decimal_places = format.slice(pos + 1).toInt();
		}
		else {
			thousands_places = format.toInt();
			decimal_places = 0;
			put_dec_point = false;
		}
		QString integer_part = "0";//QString::number((int)d);
		String decimal_part = QString::number(d, 'f', decimal_places);
		int dotpos = decimal_part.indexOf('.');
		if(dotpos < 0) {
			integer_part = decimal_part;
			decimal_part = QString();
		}
		else {
			integer_part = decimal_part.slice(0, dotpos);
			decimal_part = decimal_part.slice(dotpos + 1);
		}
		decimal_part = decimal_part.leftJustified(decimal_places, '0', true);

		int len = integer_part.length();
		if(thousands_places > 0) {
			for(int i=0; i<len; i++) {
				if(i % thousands_places == 0 && i > 0)
					ret.prepend(' ');
				ret.prepend(integer_part[len - i - 1]);
			}
		}
		else {
			ret = integer_part;
		}
		if(decimal_places > 0)
			ret += "." + decimal_part;
		else if(decimal_places == 0 && put_dec_point)
			ret += '.';
		if(negative)
			ret.prepend('-');
	}
	else
		ret = QString::number(d);
	return ret;
}

QString String::decimalNumber(double d, int decimals)
{
	//qfLogFuncFrame();
	String ret;
	if(decimals < 0)
		ret = QString::number(d);
	else {
		ret = QString::number(d, 'f', decimals);
		int dot_ix = ret.indexOf('.');
		if(dot_ix < 0) {
			if(decimals > 0)
				ret += '.' + QString().fill('0', decimals);
		}
		else {
			int dec_len = dot_ix + decimals + 1;
			ret = ret.slice(0, dec_len);
			if(ret.length() < dec_len)
				ret = ret.leftJustified(dec_len, '0');
		}
	}
	return ret;
}
