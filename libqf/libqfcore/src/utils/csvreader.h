#ifndef QF_CORE_UTILS_CSVREADER_H
#define QF_CORE_UTILS_CSVREADER_H

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
	enum {AppendEndl = 1};
protected:
	QTextStream *fTextStream;
	char fSeparator, fQuote;
	QString singleQuote, doubleQuote, charsToQuote; ///< pomocne promenne
public:
	void setSeparator(char _separator) {fSeparator = _separator;}
	void setQuote(char _quote) {fQuote = _quote;}
	void setTextStream(QTextStream *ts)
	{
		setTextStream(ts, fSeparator, fQuote);
	}
	void setTextStream(QTextStream *ts, char _separator, char _quote = '"')
	{
		fTextStream = ts;
		setSeparator(_separator);
		setQuote(_quote);
	}
	QTextStream& textStream();

	QString unquoteCSVField(const QString &s);
	QString readCSVLine();
	QStringList readCSVLineSplitted();
	QString quoteCSVField(const QString &s);
	void writeCSVLine(const QStringList &values, int option_flags = 0);
};

}}}

#endif // QF_CORE_UTILS_CSVREADER_H
