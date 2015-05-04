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

#include "ui_dlgcolumndef.h"

//#include <qf/core/sql/catalog.h>

#include <QDialog>

namespace qf {
namespace core {
namespace sql {
class FieldInfo;
}
}
}

class QSqlDatabase;

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
	void loadColumnDefinition(const qf::core::sql::FieldInfo &fi);

	void enableControls(bool v);

	bool isShowCommand();

	QString toString();
private slots:
	void on_lstRefTable_currentIndexChanged(const QString & text);
	void on_lstCharacterSet_activated(const QString & text);
public:
	DlgColumnDef(QWidget * parent, const QString& table);
	virtual ~DlgColumnDef();
};

#endif
