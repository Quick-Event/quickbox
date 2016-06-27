//
// C++ Interface: dlgaltertable
//
// Description: 
//
//
// Author: Fanda Vacek <fanda.vacek@volny.cz>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef DLGALTERTABLE_H
#define DLGALTERTABLE_H

#include "ui_dlgaltertable.h"

#include <qf/core/sql/catalog.h>

#include <QDialog>

class MainWindow;

/**
@author Fanda Vacek
*/
class DlgAlterTable : public QDialog, Ui::DlgAlterTable
{
	Q_OBJECT
	friend class DlgIndexDef;
private:
	typedef QDialog Super;
public:
	DlgAlterTable(QWidget * parent, const QString& db, const QString& tbl);
	virtual ~DlgAlterTable();

	void accept() Q_DECL_OVERRIDE;
protected:
	void refresh();
private slots:
	void on_btFieldEdit_clicked();
	void on_btFieldInsert_clicked(bool append = false);
	void on_btFieldAppend_clicked();
	void on_btFieldDelete_clicked();

	void on_btIndexAdd_clicked();
	void on_btIndexEdit_clicked();
	void on_btIndexDelete_clicked();

protected:
	MainWindow* mainWindow();
	QSqlDatabase connection();
	bool execCommand(const QString &qs);
	QString dropIndexCommand(const QString &index_name);
private:
	QString m_tableName, m_schemaName;
	QString oldComment;
};

#endif
