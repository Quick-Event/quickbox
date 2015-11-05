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
	CSVReader(QTextStream *ts = NULL, char _separator = ',', char _quote = '"');
	virtual ~CSVReader();
public:
	enum OptionFlag {AppendEndl = 1};
public:
	void setSeparator(char _separator) {m_separator = _separator;}
	void setQuote(char _quote) {m_quote = _quote;}
	void setTextStream(QTextStream *ts) { setTextStream(ts, m_separator, m_quote); }
	void setTextStream(QTextStream *ts, char _separator, char _quote = '"')
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
	QTextStream *m_textStream;
	char m_separator, m_quote;
private:
	QString m_singleQuote, m_doubleQuote, m_charsToQuote; ///< pomocne promenne
};

}}}

#endif // QF_CORE_UTILS_CSVREADER_H
