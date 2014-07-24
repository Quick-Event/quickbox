//
// C++ Interface: dlgcolumndef
//
// Description:
//
//
// Author: Fanda Vacek <fanda.vacek@volny.cz>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef DLGCOLUMNDEF_H
#define DLGCOLUMNDEF_H

#include "qfsqlcatalog.h"

#include <QDialog>

#include "ui_dlgcolumndef.h"

class DlgColumnDef : public QDialog, public Ui::DlgColumnDef
{
	Q_OBJECT
protected:
	QString dbName, tableName;
	static QMap< QString, QMap<QString, QStringList> > f_collationsCache;
protected:
	QSqlDatabase connection();
	void loadCollationsForCurrentCharset();
public:
	void clearFields();
	void loadColumnDefinition(const QFSqlFieldInfo &fi);

	void enableControls(bool v);

	bool showCommand();

	QString toString();
private slots:
	void on_lstRefTable_currentIndexChanged(const QString & text);
	void on_lstCharacterSet_activated(const QString & text);
public:
	DlgColumnDef(QWidget * parent, const QString& table);
	virtual ~DlgColumnDef();
};

#endif
