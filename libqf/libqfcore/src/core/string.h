#ifndef QF_CORE_QFSTRING_H
#define QF_CORE_QFSTRING_H

#include "coreglobal.h"

#include <QString>

#include <limits>

namespace qf {
namespace core {

class QFCORE_DECL_EXPORT String : public QString
{
public:
	String() : QString() {}
	String(const QString &str) : QString(str) {}
	String(const String &str) : QString(str) {}
public:
	/**
	* python's like slice
	* @param start from index, can be negative
	* @param end to index, cam be begative or omitted
	*/
	String slice(int start, int end = std::numeric_limits<int>::max()) const;

	int pos(QChar what_char, QChar quote = 0) const;

	//! Split string and optionaly trim splitted parts
	/// If fs ends with \a sep, an empty string is appended at the end of string list.
	/// if fs contain n \a sep chars FStringList will contain n+1 strings.
	/// Empty string produce empty string list
	/// @param trim_parts trim strings and remove quotes in  list after parsing
	QStringList splitAndTrim(QChar sep = '\t', QChar quote = 0, bool trim_parts = true, SplitBehavior keep_empty_parts = QString::SkipEmptyParts) const;
};

}
}

#endif
