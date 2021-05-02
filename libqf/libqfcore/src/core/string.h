#ifndef QF_CORE_QFSTRING_H
#define QF_CORE_QFSTRING_H

#include "coreglobal.h"

#include <QString>
#include <QStringList>

#include <limits>

namespace qf {
namespace core {

class QFCORE_DECL_EXPORT String : public QString
{
public:
	String() : QString() {}
	String(const QString &str) : QString(str) {}
public:
	static const bool TrimParts = true;
public:
	/**
	* python's like slice
	* @param start from index, can be negative
	* @param end to index, cam be begative or omitted
	*/
	String slice(int start, int end = std::numeric_limits<int>::max()) const;

	int pos(QChar what_char, QChar quote = 0) const;
	QChar value(int ix) const;

	//! Split string and optionaly trim splitted parts
	/// If fs ends with \a sep, an empty string is appended at the end of string list.
	/// if fs contain n \a sep chars FStringList will contain n+1 strings.
	/// Empty string produce empty string list
	/// @param trim_parts trim strings and remove quotes in  list after parsing
	QStringList splitAndTrim(QChar sep = '\t', QChar quote = 0, bool trim_parts = TrimParts, SplitBehavior keep_empty_parts = QString::SkipEmptyParts) const;

	//! finds matching bracket, if string don't start with opening bracket, the outermost closing bracket in the string is found..
	/// @return -1 if the opening or matching bracket is not found. String length if no brackets are found.
	int indexOfMatchingBracket(char opening_bracket = '(', char closing_bracket = ')', char quote = '\'') const;

	//! Splits string using only separators neither in brackets nor in quotes, quoted brackets are ignored.
	//! If trim parts is on, also enclosing brackets (if any) are removed form splitted parts and the content is trimmed.
	//! If part is empty (after optional trimming) and \a keep_empty_parts != \a KeepEmptyParts, part is ignored.
	//! The outermost brackets (if any) are allways ignored.
	QStringList splitBracketed(char sep = ',', char opening_bracket = '(', char closing_bracket = ')', char quote = '\0',
							   bool trim_parts = TrimParts,
							   SplitBehavior keep_empty_parts = QString::SkipEmptyParts) const;

	//! The same like splitBracketed(',', '{', '}', '\'').
	QStringList splitVector() const {return splitBracketed(',', '{', '}', '\'', TrimParts, QString::KeepEmptyParts);}

	bool toBool() const;

	/// "N(3,2)" separate thousands by space, 2 decimal places
	/// "N(0,2)" do not separate thousands by space, 2 decimal places
	/// "N(3)" separate thousands by space, 0 decimal places, no decimal point
	/// "N(3,0)" separate thousands by space, 0 decimal places, ends with decimal point
	static QString number(double d, const QString &format);
	static QString decimalNumber(double d, int decimals);
};

}
}

#endif
