
//
// Author: Frantisek Vacek <fanda.vacek@volny.cz>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef QFSQLSYNTAXHIGHLIGHTER_H
#define QFSQLSYNTAXHIGHLIGHTER_H

#include <QSet>

#include <QSyntaxHighlighter>

class QTextEdit;

//! TODO: write class documentation.
class QFSqlSyntaxHighlighter : public QSyntaxHighlighter
{
	public:
		static QSet<QString> keyWords();
	public:
		QFSqlSyntaxHighlighter(QTextEdit* parent);
		virtual ~QFSqlSyntaxHighlighter();
		
		virtual void highlightBlock(const QString &text);
};
   
#endif // QFSQLSYNTAXHIGHLIGHTER_H

