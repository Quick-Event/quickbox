#ifndef CLASSESWIDGET_H
#define CLASSESWIDGET_H

#include <QFrame>

namespace Ui {
	class ClassesWidget;
}

class ClassesWidget : public QFrame
{
	Q_OBJECT
private:
	typedef QFrame Super;
public:
	explicit ClassesWidget(QWidget *parent = 0);
	~ClassesWidget() Q_DECL_OVERRIDE;

private:
	Ui::ClassesWidget *ui;
};

#endif // CLASSESWIDGET_H
