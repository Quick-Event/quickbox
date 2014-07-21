#ifndef DLGEDITCONNECTION_H
#define DLGEDITCONNECTION_H

#include <qfexception.h>
#include "servertreeitem.h"

#include "ui_dlgeditconnection.h"
/*
namespace QFLib {
	class XmlConfigElement;
}
*/
class DlgEditConnection : public QDialog, private Ui::DlgEditConnection
{
	Q_OBJECT;
	public:
		enum OptionIndex {OptionIndexMySql = 0, OptionIndexSqlite};
private slots:
	void on_btOk_clicked();
	void on_btCancel_clicked() {reject();}
	void on_cbxDriver_currentIndexChanged(const QString &s);
	void on_btFindDatabaseFile_clicked();
	protected:
	QStringList drivers;
	QStringList codecs;
	static QStringList mysqlCodecs;
	//QFXmlTreeModel* model();
	Connection *currConnection;
public:
    DlgEditConnection(QWidget *parent = 0);
	void setContent(Connection& connection)  throw(QFException);
};

#endif // DLGEDITCONNECTION_H
