#ifndef QF_CORE_UTILS_CSVREADER_H
#define QF_CORE_UTILS_CSVREADER_H

#include "../core/coreglobal.h"
#include "../core/utils.h"

class QTextStream;
class QStringList;

namespace qf {
namespace core {
namespace utils {

class QFCORE_DECL_EXPORT CSVReader
{
public:
	CSVReader(QTextStream *ts = nullptr, char _separator = ',', char _quote = '"');
	virtual ~CSVReader();
public:
	enum OptionFlag {AppendEndl = 1};
public:
	void setSeparator(QChar _separator) {m_separator = _separator;}
	void setQuote(QChar _quote) {m_quote = _quote;}
	void setLineComment(QChar comment_char) {m_lineComment = comment_char;}
	void setTextStream(QTextStream *ts) { setTextStream(ts, m_separator, m_quote); }
	void setTextStream(QTextStream *ts, QChar _separator, QChar _quote = '"')
	{
		m_textStream = ts;
		setSeparator(_separator);
		setQuote(_quote);
	}
	QTextStream& textStream();

	QString unquoteCSVField(const QString &s);
	QString readCSVLine();
	QStringList readCSVLineSplitted();
	QString quoteCSVField(const QString &s);
	void writeCSVLine(const QStringList &values, int option_flags = 0);
protected:
	QString readCSVLine1();
protected:
	QTextStream *m_textStream;
	QChar m_separator, m_quote, m_lineComment;
private:
	QString m_singleQuote, m_doubleQuote, m_charsToQuote; ///< pomocne promenne
};

}}}

#endif // QF_CORE_UTILS_CSVREADER_H
