#include <qf/core/string.h>

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

QStringList String::splitAndTrim(QChar sep, QChar quote, bool trim_parts, QString::SplitBehavior keep_empty_parts) const
{
	QStringList ret;
	String fs = *this, s;
	bool first_scan = true;
	while(!fs.isEmpty()) {
		if(first_scan)
			first_scan = false;
		else
			fs = fs.slice(1); // remove separator
		int ix = fs.pos(sep, quote);
		if(ix >= 0) {
			s = fs.slice(0, ix);
			fs = fs.slice(ix);
		}
		else {
			s = fs;
			fs = QString();
		}
		if(trim_parts) {
			s = s.trimmed();
			if(s[0] == quote && s[-1] == quote)
				s = s.slice(1, -1);
		}
		if(!s.isEmpty()) ret.append(s);
		else if(keep_empty_parts == KeepEmptyParts) ret.append(s);
	}
	return ret;
}
