
//
// Author: Frantisek Vacek <fanda.vacek@volny.cz>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef DLGINDEXDEF_H
#define DLGINDEXDEF_H

#include <qf/core/log.h>

#include <QDialog>

namespace Ui {class DlgIndexDef;}

class QSqlDatabase;


class  DlgIndexDef : public QDialog
{
	Q_OBJECT
	private:
		Ui::DlgIndexDef *ui;
	protected:
		QString dbName, tableName, indexName;

		//MainWindow* mainWindow();
		QSqlDatabase connection();

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

