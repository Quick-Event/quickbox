#ifndef CLASSESWIDGET_H
#define CLASSESWIDGET_H

#include "thispartwidget.h"
#include <QFrame>

namespace Ui {
	class ClassesWidget;
}

struct CourseDef;

class ClassesWidget : public QFrame
{
	Q_OBJECT
private:
	typedef QFrame Super;
public:
	explicit ClassesWidget(QWidget *parent = 0);
	~ClassesWidget() Q_DECL_OVERRIDE;

	void settleDownInPartWidget(ThisPartWidget *part_widget);
private:
	Q_SLOT void import_ocad();
	Q_SLOT void reload();

	void createClassesCourses(int current_stage, const QList<CourseDef> &courses);

private:
	Ui::ClassesWidget *ui;
};

#endif // CLASSESWIDGET_H
