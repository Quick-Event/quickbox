#ifndef FINDREGISTRATIONEDIT_H
#define FINDREGISTRATIONEDIT_H

#include <QLineEdit>

class FindRegistrationEdit : public QLineEdit
{
	Q_OBJECT
private:
	typedef QLineEdit Super;
public:
	FindRegistrationEdit(QWidget *parent = nullptr);
private:
	void onEditTextChanged(const QString &new_text);
};

#endif // FINDREGISTRATIONEDIT_H
