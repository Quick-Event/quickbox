// sqltextedit.cpp: implementation of the SqlTextEdit class.
//
//////////////////////////////////////////////////////////////////////

#include "sqltextedit.h"

//#include <qfstring.h>
#include "qfsqlsyntaxhighlighter.h"

#include <qf/core/utils.h>

#include <QKeyEvent>
#include <QTextCursor>
#include <QAbstractItemView>
#include <QScrollBar>
#include <QStringListModel>

#include <qf/core/log.h>

//=============================================================
//                                             SqlTextEditCompleter
//=============================================================
QString SqlTextEditCompleter::pathFromIndex(const QModelIndex &index) const
{
	QAbstractItemModel *m = model();
	if(!m) return QString();
	QStringList sl;
	QModelIndex ix = index;
	while(ix.isValid()) {
		sl.prepend(m->data(ix).toString());
		ix = ix.parent();
	}
	QString ret = sl.join(".");
	qfDebug() << Q_FUNC_INFO << ret;
	return ret;
}

QStringList SqlTextEditCompleter::splitPath(const QString &path) const
{
	QStringList ret = path.split('.');
	qfDebug() << Q_FUNC_INFO << ret.join(".");
	return ret;
}

//=============================================================
//                                            SqlTextEdit
//=============================================================
SqlTextEdit::SqlTextEdit(QWidget * parent)
	: QTextEdit(parent), f_completer(nullptr), f_completionModel(nullptr)
{
	new QFSqlSyntaxHighlighter(this);
	connect(this, SIGNAL(textChanged()), this, SLOT(slotTextChanged()), Qt::QueuedConnection); ///musi byt queued v 4.7, jinak mi to hazelo SEG fault po backspace
	/*
	setPlainText(tr("This TextEdit provides autocompletions for words that have more than"
			" 3 characters. You can trigger autocompletion using ") +
			QKeySequence("Ctrl+E").toString(QKeySequence::NativeText));
	*/
	f_completer = new SqlTextEditCompleter(this);
	//QStringList sl = QFSqlSyntaxHighlighter::keyWords().toList();
	//qSort(sl);
	//foreach(QString s, sl) qfInfo() << s;
	//f_completer->setModel(new QStringListModel(sl, this));
	//f_completer->setModelSorting(QCompleter::CaseInsensitivelySortedModel);
	f_completer->setCaseSensitivity(Qt::CaseInsensitive);
	setCompleter(f_completer);
}

SqlTextEdit::~SqlTextEdit()
{
}

void SqlTextEdit::setCompletionModel(QAbstractItemModel *m)
{
	QAbstractItemModel *old_m = f_completionModel;
	f_completionModel = m;
	f_completer->setModel(f_completionModel);
	//if(m) m->setParent(this);
	QF_SAFE_DELETE(old_m);
}

void SqlTextEdit::slotTextChanged()
{
	qfLogFuncFrame();
	static QString orig_word;
	QTextCursor c = textCursor();
	/*
	qfDebug() << "\t text cursor pos:" << c.position() << "is NULL:" << c.isNull();
	{
		QTextDocument *doc = document();
		qfDebug() << "\t doc:" << doc << "block count:" << doc->blockCount() << "line count:" << doc->lineCount();
	}
	*/
	int curs_pos = c.position();
	//c.movePosition(QTextCursor::Left);
	c.movePosition(QTextCursor::StartOfWord);
	qfDebug() << "\t start word pos:" << c.position();
	c.movePosition(QTextCursor::EndOfWord, QTextCursor::KeepAnchor);
	qfDebug() << "\t end word pos:" << c.position();
	if(c.position() == curs_pos) {
		/// men klicova slova na uppercase jen pri psani, ne pri editaci
		QString word = c.selectedText();
		qfDebug().nospace() << "\t '" << word << "'";
		if(!word.isEmpty()) {
			QString upword = word.toUpper();
			if(QFSqlSyntaxHighlighter::keyWords().contains(upword)) {
				if(word != upword) {
					c.removeSelectedText();
					c.insertText(upword);
					orig_word = word;
				}
			}
			else {
				if(!orig_word.isEmpty() && upword.startsWith(orig_word.toUpper())) {
					//qfDebug().noSpace() << "\torig: '" << orig_word << "'";
					word = orig_word + word.mid(orig_word.length());
					//qfDebug().noSpace() << "\tnew: '" << word << "'";
					//qfDebug() << "\tDELETE";
					orig_word = QString();
					c.removeSelectedText();
					//qfDebug() << "\tINSERT";
					c.insertText(word);
				}
			}
		}
	}
}

void SqlTextEdit::setCompleter(QCompleter *c)
{
	if (f_completer)
		QObject::disconnect(f_completer, 0, this, 0);

	f_completer = c;

	if (!f_completer) return;

	f_completer->setWidget(this);
	f_completer->setCompletionMode(QCompleter::PopupCompletion);
	f_completer->setCaseSensitivity(Qt::CaseInsensitive);
	QObject::connect(f_completer, SIGNAL(activated(const QString&)), this, SLOT(insertCompletion(const QString&)));
}

 QCompleter* SqlTextEdit::completer() const
{
	return f_completer;
}

 void SqlTextEdit::insertCompletion(const QString& completion)
{
	QTextCursor tc = textCursor();
	int extra = completion.length() - f_completer->completionPrefix().length();
	tc.insertText(completion.right(extra));
	tc.movePosition(QTextCursor::EndOfWord);
	setTextCursor(tc);
}

 QString SqlTextEdit::textUnderCursor() const
{
	qfLogFuncFrame();
	QTextCursor tc = textCursor();
	int pos = tc.position();
	QStringList sl;
	while(true) {
		tc.select(QTextCursor::WordUnderCursor);
		QString s = tc.selectedText();
		qfDebug() << "\tselection start: " << tc.selectionStart() << "end:" << tc.selectionEnd();
		if(tc.selectionEnd() < pos) {
			/// kdyz selekce slova konci pred mistem, kde byl kurzor, znamena to, ze slovo koncilo tyeckou a kurzor byl tesne za ni.
			/// tak tecku pridame.
			s += '.';
			pos = -1;
		}
		tc.setPosition(tc.selectionStart());
		tc.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor);
		qfDebug().nospace() << "\tselected: '" << s << "'";
		qfDebug().nospace() << "\tdot: '" << tc.selectedText() << "'";
		if(tc.selectedText() != ".") {
			if(!s.isEmpty()) sl.prepend(s);
			break;
		}
		qfDebug().nospace() << "\tprepending: '" << s << "'";
		sl.prepend(s);
	}
	QString ret = sl.join(".").replace("..", ".");
	qfDebug() << "\tret:" << ret;
	return ret;
}

 void SqlTextEdit::keyPressEvent(QKeyEvent *e)
{
	if((e->key() == Qt::Key_Return && e->modifiers() == Qt::ShiftModifier) || (e->key() == Qt::Key_Enter && e->modifiers() == Qt::KeypadModifier)) {
		//qDebug() << "doSql() emited.";
		emit doSql();
		e->accept();
		return;
	}

	if (f_completer && f_completer->popup()->isVisible()) {
         /// The following keys are forwarded by the completer to the widget
		switch (e->key()) {
			case Qt::Key_Enter:
			case Qt::Key_Return:
			case Qt::Key_Escape:
			case Qt::Key_Tab:
			case Qt::Key_Backtab:
			//case Qt::Key_Delete:
				e->ignore();
				return; /// let the completer do default behavior
			default:
				break;
		}
	}

	bool is_shortcut = ((e->modifiers() & Qt::ControlModifier) && e->key() == Qt::Key_Space); /// CTRL+space
	if (!f_completer || !is_shortcut) {/// dont process the shortcut when we have a completer
		QTextEdit::keyPressEvent(e);
	}
	const bool ctrl_or_shift = e->modifiers() & (Qt::ControlModifier | Qt::ShiftModifier);
	if (!f_completer || (ctrl_or_shift && e->text().isEmpty())) return;

	static QString eow("~!@#$%^&*()_+{}|:\"<>?,/;'[]\\-="); /// end of word
	bool has_modifier = (e->modifiers() != Qt::NoModifier) && !ctrl_or_shift;
	QString completion_prefix = textUnderCursor();
	qfDebug() << "completition prefix:" << completion_prefix;
	if (!is_shortcut && (has_modifier || e->text().isEmpty()|| completion_prefix.length() < 3 || eow.contains(e->text().right(1)))) {
		f_completer->popup()->hide();
		return;
	}

	qfDebug() << "completer completition prefix:" << f_completer->completionPrefix();
	if (completion_prefix != f_completer->completionPrefix()) {
		f_completer->setCompletionPrefix(completion_prefix);
		f_completer->popup()->setCurrentIndex(f_completer->completionModel()->index(0, 0));
	}
	qfDebug() << "completer new completition prefix:" << f_completer->completionPrefix();
	QRect cr = cursorRect();
	cr.setWidth(f_completer->popup()->sizeHintForColumn(0) + f_completer->popup()->verticalScrollBar()->sizeHint().width());
	qfDebug() << "popup it up!" << f_completer->model()->rowCount();
	qfDebug() << "popup it up!" << f_completer->completionModel()->rowCount();
	f_completer->complete(cr); /// pop it up!
}

