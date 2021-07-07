#ifndef CLASSESTABLEVIEW_H
#define CLASSESTABLEVIEW_H

#include <qf/qmlwidgets/tableview.h>

class ClassesTableView : public qf::qmlwidgets::TableView
{
	Q_OBJECT
private:
	typedef qf::qmlwidgets::TableView Super;
public:
	ClassesTableView(QWidget *parent = nullptr);

	void insertRow() Q_DECL_OVERRIDE;
	void removeSelectedRows() Q_DECL_OVERRIDE;
};

#endif // CLASSESTABLEVIEW_H
