
//
// Author: Frantisek Vacek <fanda.vacek@volny.cz>, (C) 2012
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef COLUMNSELECTORWIDGET_H
#define COLUMNSELECTORWIDGET_H


#include <qf/qmlwidgets/framework/dialogwidget.h>

class QSqlDatabase;
namespace Ui {class ColumnSelectorWidget;};

//! TODO: write class documentation.
class  ColumnSelectorWidget : public qf::qmlwidgets::framework::DialogWidget
{
	Q_OBJECT
	Q_PROPERTY(QString persistentSettingsId READ persistentSettingsId WRITE setPersistentSettingsId)
private:
	typedef qf::qmlwidgets::framework::DialogWidget Super;
	Ui::ColumnSelectorWidget *ui;
protected:
	QString m_tableName;
protected slots:
	void lazyInit();
	void on_btAll_clicked();
	void on_btInvert_clicked();
	void on_btPasteSelectedColumns_clicked();
signals:
	void columnNamesCopiedToClipboard(const QString &column_names);
public:
	ColumnSelectorWidget(QString table_name, const QSqlDatabase &conn, QWidget *parent = NULL);
	virtual ~ColumnSelectorWidget();
};

#endif // COLUMNSELECTORWIDGET_H

