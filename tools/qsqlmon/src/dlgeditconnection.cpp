#include "dlgeditconnection.h"
#include "theapp.h"

#include <qf/core/log.h>

//#include <qfstring.h>
//#include <qfapplication.h>
//#include <qfxmlconfigdocument.h>

#include <QFileDialog>
#include <QVariant>
#include <QTextCodec>

//#define QF_NO_TRASH_OUTPUT
	
QStringList DlgEditConnection::mysqlCodecs;

DlgEditConnection::DlgEditConnection(QWidget *parent) :
	QDialog(parent, Qt::WindowSystemMenuHint | Qt::WindowMinMaxButtonsHint)
{
	currConnection = NULL;
	setupUi(this);
	drivers = QSqlDatabase::drivers();
	//drivers << "QMYSQL" << "QFMYSQL" << "QPSQL" << "QFPSQL" << "QSQLITE" << "QFSQLITE" << "QIBASE" << "QFIBASE";
	cbxDriver->addItems(drivers);

	if(codecs.isEmpty()) {
		static const QString s = "System,UTF-8,Windows-1250,UTF-16,UTF-16BE,UTF-16LE,ISO 8859-1,ISO 8859-2,ISO 8859-3,ISO 8859-4,ISO 8859-5,ISO 8859-6,ISO 8859-7,ISO 8859-8,ISO 8859-9,ISO 8859-10,ISO 8859-13,ISO 8859-14,ISO 8859-15,ISO 8859-16,Windows-1251,Windows-1252,Windows-1253,Windows-1254,Windows-1255,Windows-1256,Windows-1257,Windows-1258,Apple Roman,Big5,Big5-HKSCS,EUC-JP,EUC-KR,GB18030-0,IBM 850,IBM 866,IBM 874,ISO 2022-JP,Iscii-Bng,Iscii-Dev,Iscii-Gjr,Iscii-Knd,Iscii-Mlm,Iscii-Ori,Iscii-Pnj,Iscii-Tlg,Iscii-Tml,JIS X 0201,JIS X 0208,KOI8-R,KOI8-U,MuleLao-1,ROMAN8,Shift-JIS,TIS-620,TSCII,WINSAMI2";
		codecs = s.split(',');
	}
	lstCodec->addItems(codecs);
	
	if(mysqlCodecs.isEmpty()) {
		QString s = "<no change>,utf8,cp1250,latin1,latin2";
		mysqlCodecs = s.split(',');
	}
	lstMySqlSetNames->addItems(mysqlCodecs);
	//codecs = QTextCodec::availableCodecs();
	//foreach(QByteArray ba, codecs) lstCodec->addItem(QString(ba));

	btFindDatabaseFile->setVisible(false);
}

void DlgEditConnection::setContent(Connection& connection)
{
	//XmlConfigElement el = connection.params;
	currConnection = &connection;
	edCaption->setText(currConnection->param("description").toString());
	edHost->setText(currConnection->param("host").toString());
	edPort->setValue(currConnection->param("port").toInt());
	edUser->setText(currConnection->param("user").toString());
	edPassword->setText(currConnection->param("password").toString());
	edDatabase->setText(currConnection->param("database").toString());
	QString s = currConnection->param("driver").toString();
	cbxDriver->setCurrentIndex(drivers.indexOf(s));
	//stackOptions->setVisible(false);
	if(s.endsWith("MYSQL")) {
		//stackOptions->setVisible(true);
		//stackOptions->setCurrentIndex(OptionIndexMySql);
		s = currConnection->param("mysqlSetNames").toString();
		//qfInfo() << s;
		lstMySqlSetNames->setCurrentIndex(mysqlCodecs.indexOf(s));
	}
	else if(s.endsWith("SQLITE")) {
		//stackOptions->setVisible(true);
		//stackOptions->setCurrentIndex(OptionIndexMySql);
		s = currConnection->param("sqlite_pragma_short_column_names").toString();
		sqlite_chkPragma_short_column_names->setChecked(s == "1");
		s = currConnection->param("sqlite_pragma_full_column_names").toString();
		sqlite_chkPragma_full_column_names->setChecked(s == "1");
	}
	s = currConnection->param("textcodec").toString();
	lstCodec->setCurrentIndex(codecs.indexOf(s));
}

void DlgEditConnection::on_btOk_clicked()
{
	if(!currConnection) {
		qfError() << "DlgEditConnection::on_btOk_clicked() - currConnection is NULL";
		reject();
	}
	currConnection->setParam("description", edCaption->text());
	currConnection->setParam("driver", cbxDriver->currentText());
	currConnection->setParam("host", edHost->text());
	currConnection->setParam("port", edPort->value());
	currConnection->setParam("user", edUser->text());
	currConnection->setParam("password", edPassword->text());
	currConnection->setParam("database", edDatabase->text());
	currConnection->setParam("textcodec", lstCodec->currentText());
	currConnection->setParam("mysqlSetNames", lstMySqlSetNames->currentText());
	currConnection->setParam("sqlite_pragma_short_column_names", ((sqlite_chkPragma_short_column_names->isChecked())? "1": "0"));
	currConnection->setParam("sqlite_pragma_full_column_names", ((sqlite_chkPragma_full_column_names->isChecked())? "1": "0"));
	accept();
}

void DlgEditConnection::on_cbxDriver_currentIndexChanged(const QString &s)
{
	//qfTrash() << QF_FUNC_NAME << s << QFSqlConnectionBase::defaultPort(s);
	//if(edPort->value() == 0)
	//	edPort->setValue(QString::number(QFSqlConnectionBase::defaultPort(s)));
	btFindDatabaseFile->setVisible(s.endsWith("SQLITE") || s.endsWith("IBASE"));
	stackOptions->hide();
	if(s.endsWith("SQLITE")) {
		stackOptions->show();
		stackOptions->setCurrentIndex(OptionIndexSqlite);
	}
	else if(s.endsWith("QFMYSQL")) {
		stackOptions->show();
		stackOptions->setCurrentIndex(OptionIndexMySql);
	}
}

void DlgEditConnection::on_btFindDatabaseFile_clicked()
{
	QString s = qfApp()->getOpenFileName (this, tr("Open database file"));
	if(!s.isEmpty()) edDatabase->setText(s);
}
