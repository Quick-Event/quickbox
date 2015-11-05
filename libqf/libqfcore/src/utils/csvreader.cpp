#include "csvreader.h"
#include "../core/log.h"
#include "../core/string.h"

#include <QTextStream>

using namespace qf::core::utils;

CSVReader::CSVReader(QTextStream *ts, char _separator, char _quote)
	: m_textStream(ts), m_separator(_separator), m_quote(_quote)
{
}

CSVReader::~CSVReader()
{
}

QTextStream &CSVReader::textStream()
{
	if(!m_textStream)
		qfFatal("DataStream is NULL.");
	return *m_textStream;
}

QString CSVReader::unquoteCSVField(const QString &s)
{
	if(m_doubleQuote.isEmpty())
		m_doubleQuote = QString("%1%2").arg(m_quote).arg(m_quote);
	if(m_singleQuote.isEmpty())
		m_singleQuote = QString("%1").arg(m_quote);
	String ret = s;
	if(ret[0] == m_quote)
		ret = ret.slice(1, -1);
	ret = ret.replace(m_doubleQuote, m_singleQuote);
	return ret;
}

QString CSVReader::readCSVLine()
{
	QString ret;
	int qcnt = 0;
	do {
		QString s = textStream().readLine();
		qcnt += s.count(m_quote);
		ret += s;
		if(qcnt % 2)
			ret += "\n";
	} while((qcnt % 2) && !textStream().atEnd());
	return ret;
}

QStringList CSVReader::readCSVLineSplitted()
{
	String s = readCSVLine();
	QStringList sl = s.splitAndTrim(m_separator, m_quote, String::TrimParts, QString::KeepEmptyParts);
	QStringList ret;
	foreach(s, sl)
		ret << unquoteCSVField(s);
	return ret;
}

static int indexOfOneOf(const QString &str, const QString &chars, char quote = '\0')
{
	bool in_quotes = false;
	for(int i=0; i<str.length(); i++) {
		if(quote && str[i] == quote)
			in_quotes = !in_quotes;
		if(!in_quotes)
			for(int j=0; j<chars.length(); j++)
				if(str[i] == chars[j])
					return i;
	}
	return -1;
}

QString CSVReader::quoteCSVField(const QString &s)
{
	if(m_doubleQuote.isEmpty())
		m_doubleQuote = QString("%1%2").arg(m_quote).arg(m_quote);
	if(m_singleQuote.isEmpty())
		m_singleQuote = QString("%1").arg(m_quote);
	if(m_charsToQuote.isEmpty())
		m_charsToQuote = QString("%1%2%3%4").arg(m_separator).arg('\r').arg('\n').arg('#');
	QString ret = s;
	ret = ret.replace(m_singleQuote, m_doubleQuote);
	if(indexOfOneOf(ret, m_charsToQuote) >= 0) {
		ret = m_singleQuote + ret + m_singleQuote;
	}
	return ret;
}


void CSVReader::writeCSVLine(const QStringList &values, int option_flags)
{
	int i = 0;
	foreach(QString s, values) {
		if(i++ > 0) textStream() << m_separator;
		textStream() << quoteCSVField(s);
	}
	if(option_flags & AppendEndl)
		textStream() << '\n';
}
