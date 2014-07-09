#include "csvreader.h"
#include "../core/log.h"
#include "../core/string.h"

#include <QTextStream>

using namespace qf::core::utils;

CSVReader::CSVReader(QTextStream *ts, char _separator, char _quote)
	: fTextStream(ts), fSeparator(_separator), fQuote(_quote)
{
}

CSVReader::~CSVReader()
{
}

QTextStream &CSVReader::textStream()
{
	if(!fTextStream)
		qfFatal("DataStream is NULL.");
	return *fTextStream;
}

QString CSVReader::unquoteCSVField(const QString &s)
{
	if(doubleQuote.isEmpty()) doubleQuote = QString("%1%2").arg(fQuote).arg(fQuote);
	if(singleQuote.isEmpty()) singleQuote = QString("%1").arg(fQuote);
	String ret = s;
	if(ret[0] == fQuote) {
		ret = ret.slice(1, -1);
	}
	ret = ret.replace(doubleQuote, singleQuote);
	return ret;
}

QString CSVReader::readCSVLine()
{
	QString ret;
	int qcnt = 0;
	do {
		QString s = textStream().readLine();
		qcnt += s.count(fQuote);
		ret += s;
		if(qcnt % 2) ret += "\n";
	} while((qcnt % 2) && !textStream().atEnd());
	return ret;
}

QStringList CSVReader::readCSVLineSplitted()
{
	String s = readCSVLine();
	QStringList sl = s.splitAndTrim(fSeparator, fQuote, String::TrimParts, QString::KeepEmptyParts);
	QStringList ret;
	foreach(s, sl) ret << unquoteCSVField(s);
	return ret;
}

static int indexOfOneOf(const QString &str, const QString &chars, char quote = '\0')
{
	bool in_quotes = false;
	for(int i=0; i<str.length(); i++) {
		if(quote && str[i] == quote) {
			in_quotes = !in_quotes;
		}
		if(!in_quotes) {
			for(int j=0; j<chars.length(); j++) {
				if(str[i] == chars[j]) return i;
			}
		}
	}
	return -1;
}

QString CSVReader::quoteCSVField(const QString &s)
{
	if(doubleQuote.isEmpty())
		doubleQuote = QString("%1%2").arg(fQuote).arg(fQuote);
	if(singleQuote.isEmpty())
		singleQuote = QString("%1").arg(fQuote);
	if(charsToQuote.isEmpty())
		charsToQuote = QString("%1%2%3%4").arg(fSeparator).arg('\r').arg('\n').arg('#');
	QString ret = s;
	ret = ret.replace(singleQuote, doubleQuote);
	if(indexOfOneOf(ret, charsToQuote) >= 0) {
		ret = singleQuote + ret + singleQuote;
	}
	return ret;
}


void CSVReader::writeCSVLine(const QStringList &values, int option_flags)
{
	int i = 0;
	foreach(QString s, values) {
		if(i++ > 0) textStream() << fSeparator;
		textStream() << quoteCSVField(s);
	}
	if(option_flags & AppendEndl)
		textStream() << '\n';
}
