
//
// Author: Frantisek Vacek <fanda.vacek@volny.cz>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef DLGINDEXDEF_H
#define DLGINDEXDEF_H

//#include <qfglobal.h>
#include <qfsqlconnection.h>


#include <QDialog>

//#define QF_NO_TRASH_OUTPUT
#include <qflogcust.h>

namespace Ui {class DlgIndexDef;};

//! TODO: write class documentation.
class  DlgIndexDef : public QDialog
{
	Q_OBJECT
	private:
		Ui::DlgIndexDef *ui;
	protected:
		QString dbName, tableName, indexName;

		//MainWindow* mainWindow();
		QFSqlConnection connection();

		void loadIndexDefinition();
	public:
		QString createIndexCommand();
	public slots:
		void on_actionAddFieldToIndex_triggered();
		void on_actionRemoveFieldFromIndex_triggered();
	public:
		DlgIndexDef(QWidget * parent, const QString &table_name, const QString &index_name = QString());
		virtual ~DlgIndexDef();
};

#endif // DLGINDEXDEF_H

