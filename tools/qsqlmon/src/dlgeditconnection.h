#ifndef DLGEDITCONNECTION_H
#define DLGEDITCONNECTION_H

#include "servertreeitem.h"

#include "ui_dlgeditconnection.h"

class DlgEditConnection : public QDialog, private Ui::DlgEditConnection
{
	Q_OBJECT
public:
	enum OptionIndex {OptionIndexMySql = 0, OptionIndexSqlite};
public:
	DlgEditConnection(QWidget *parent = nullptr);
private slots:
	void on_btOk_clicked();
	void on_btCancel_clicked() {reject();}
	void on_cbxDriver_currentIndexChanged(const QString &s);
	void on_btFindDatabaseFile_clicked();
protected:
	QStringList drivers;
	QStringList codecs;
	static QStringList mysqlCodecs;
	Connection::Params m_connectionParams;
public:
	void setParams(const Connection::Params &params);
	const Connection::Params& params();
};

#endif // DLGEDITCONNECTION_H
