// sqltextedit.h: interface for the SqlTextEdit class.
//
//////////////////////////////////////////////////////////////////////

#ifndef SQLTEXTEDIT_H__INCLUDED
#define SQLTEXTEDIT_H__INCLUDED

#include <QTextEdit>
#include <QCompleter>

class QKeyEvent;
class QAbstractItemModel;

class SqlTextEditCompleter : public QCompleter
{
	public:
		SqlTextEditCompleter(QObject *parent = nullptr) : QCompleter(parent) {}
		virtual ~SqlTextEditCompleter() {}

		virtual QString pathFromIndex(const QModelIndex &index) const;
		virtual QStringList splitPath(const QString &path) const;
};

class SqlTextEdit : public QTextEdit
{
	Q_OBJECT;
protected:
	QCompleter *f_completer;
	QAbstractItemModel *f_completionModel;
protected:
	virtual void keyPressEvent(QKeyEvent *e);
	QString textUnderCursor() const;
private slots:
	void insertCompletion(const QString &completion);
signals:
	void doSql();
protected slots:
	void slotTextChanged();
public:
	void setCompleter(QCompleter *c);
	QCompleter *completer() const;

	//! widget takes ownership of model.
	void setCompletionModel(QAbstractItemModel *m);
public:
	SqlTextEdit( QWidget * parent = 0);
	virtual ~SqlTextEdit();
};

#endif // !defined(SQLTEXTEDIT_H__INCLUDED)
