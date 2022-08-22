
//
// Author: Frantisek Vacek <fanda.vacek@volny.cz>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//

#include "qfsqlsyntaxhighlighter.h"

#include <qf/core/log.h>

#include <QRegularExpression>
#include <QTextEdit>

QFSqlSyntaxHighlighter::QFSqlSyntaxHighlighter(QTextEdit *parent)
	: QSyntaxHighlighter(parent)
{
}

QFSqlSyntaxHighlighter::~QFSqlSyntaxHighlighter()
{
}

QSet<QString> QFSqlSyntaxHighlighter::keyWords()
{
	static QSet<QString> list;
	if(list.isEmpty()) {
#include "sql-keywords.h"
	}
	return list;
} 

void QFSqlSyntaxHighlighter::highlightBlock(const QString &_text)
{
	QString text = _text + " ";
	
	QTextCharFormat commentFormat;
	QTextCharFormat keywordFormat;
	keywordFormat.setFontWeight(QFont::Bold);
	commentFormat.setForeground(Qt::darkCyan);
	QString patt_comment = "^--";

	QRegularExpression expression(patt_comment);
	int index = text.indexOf(expression);
	if(index == 0) {
		setFormat(0, text.length(), commentFormat);
	}
	else {
		/// take all words and check if they are not keywords
		int prev_i = 0;
		for(int i=0; i<text.length(); i++) {
			if(text[i].isSpace()) {
				if(i == prev_i) continue;
				if(i == prev_i+1) {prev_i = i; continue;}
				else {
					QString s = text.mid(prev_i, i-prev_i).trimmed();
					//qfTrash() << s;
					if(keyWords().contains(s)) {
						//qfTrash() << "\tfound";
						setFormat(prev_i, i-prev_i, keywordFormat);
					}
				}
				prev_i = i;
			}
		}
	}
}
