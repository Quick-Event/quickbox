#include "dlgeditconnection.h"
#include "theapp.h"

#include <qf/core/log.h>
#include <qf/qmlwidgets/dialogs/filedialog.h>

#include <QFileDialog>
#include <QVariant>

//#define QF_NO_TRASH_OUTPUT
	
QStringList DlgEditConnection::mysqlCodecs;

DlgEditConnection::DlgEditConnection(QWidget *parent) :
	QDialog(parent, Qt::WindowSystemMenuHint | Qt::WindowMinMaxButtonsHint)
{
	qfLogFuncFrame();
	setupUi(this);
	btFindDatabaseFile->setVisible(false);
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
}

void DlgEditConnection::setParams(const Connection::Params &params)
{
	//XmlConfigElement el = connection.params;
	m_connectionParams = params;
	edCaption->setText(m_connectionParams.param("description").toString());
	edHost->setText(m_connectionParams.param("host").toString());
	edPort->setValue(m_connectionParams.param("port").toInt());
	edUser->setText(m_connectionParams.param("user").toString());
	edPassword->setText(m_connectionParams.param("password").toString());
	edDatabase->setText(m_connectionParams.param("database").toString());
	QString s = m_connectionParams.param("driver").toString();
	cbxDriver->setCurrentIndex(drivers.indexOf(s));
	//stackOptions->setVisible(false);
	if(s.endsWith("MYSQL")) {
		//stackOptions->setVisible(true);
		//stackOptions->setCurrentIndex(OptionIndexMySql);
		s = m_connectionParams.param("mysqlSetNames").toString();
		//qfInfo() << s;
		lstMySqlSetNames->setCurrentIndex(mysqlCodecs.indexOf(s));
	}
	else if(s.endsWith("SQLITE")) {
		//stackOptions->setVisible(true);
		//stackOptions->setCurrentIndex(OptionIndexMySql);
		s = m_connectionParams.param("sqlite_pragma_short_column_names").toString();
		sqlite_chkPragma_short_column_names->setChecked(s == "1");
		s = m_connectionParams.param("sqlite_pragma_full_column_names").toString();
		sqlite_chkPragma_full_column_names->setChecked(s == "1");
	}
	s = m_connectionParams.param("textcodec").toString();
	lstCodec->setCurrentIndex(codecs.indexOf(s));
}

const Connection::Params &DlgEditConnection::params()
{
	return m_connectionParams;
}

void DlgEditConnection::on_btOk_clicked()
{
	/*
	if(!currConnection) {
		qfError() << "DlgEditConnection::on_btOk_clicked() - currConnection is NULL";
		reject();
	}
	*/
	m_connectionParams.setParam("description", edCaption->text());
	m_connectionParams.setParam("driver", cbxDriver->currentText());
	m_connectionParams.setParam("host", edHost->text());
	m_connectionParams.setParam("port", edPort->value());
	m_connectionParams.setParam("user", edUser->text());
	m_connectionParams.setParam("password", edPassword->text());
	m_connectionParams.setParam("database", edDatabase->text());
	m_connectionParams.setParam("textcodec", lstCodec->currentText());
	m_connectionParams.setParam("mysqlSetNames", lstMySqlSetNames->currentText());
	m_connectionParams.setParam("sqlite_pragma_short_column_names", ((sqlite_chkPragma_short_column_names->isChecked())? "1": "0"));
	m_connectionParams.setParam("sqlite_pragma_full_column_names", ((sqlite_chkPragma_full_column_names->isChecked())? "1": "0"));
	accept();
}

void DlgEditConnection::on_cbxDriver_currentIndexChanged(const QString &s)
{
	qfLogFuncFrame() << s << (s.endsWith("SQLITE") || s.endsWith("IBASE"));
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
	QString s = edDatabase->text();
	s = qf::qmlwidgets::dialogs::FileDialog::getOpenFileName(this, tr("Open database file"), s);
	if(!s.isEmpty())
		edDatabase->setText(s);
}
