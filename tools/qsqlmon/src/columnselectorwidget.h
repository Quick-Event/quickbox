
//
// Author: Frantisek Vacek <fanda.vacek@volny.cz>, (C) 2012
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef COLUMNSELECTORWIDGET_H
#define COLUMNSELECTORWIDGET_H


#include <qf/qmlwidgets/framework/partwidget.h>

class QFSqlConnection;
namespace Ui {class ColumnSelectorWidget;};

//! TODO: write class documentation.
class  ColumnSelectorWidget : public qf::qmlwidgets::framework::PartWidget
{
	Q_OBJECT
private:
	typedef qf::qmlwidgets::framework::PartWidget Super;
	Ui::ColumnSelectorWidget *ui;
protected:
	QString f_tableName;
protected slots:
	void lazyInit();
	void on_btAll_clicked();
	void on_btInvert_clicked();
	void on_btPasteSelectedColumns_clicked();
signals:
	void columnNamesCopiedToClipboard(const QString &column_names);
public:
	ColumnSelectorWidget(QString table_name, QFSqlConnection &conn, QWidget *parent = NULL);
	virtual ~ColumnSelectorWidget();
};

#endif // COLUMNSELECTORWIDGET_H

