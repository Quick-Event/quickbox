#include "theapp.h"
#include "mainwindow.h"

#include <qf/core/log.h>
#include <qf/core/logdevice.h>

//#include <QDialog>
#include <Qt>
#include <QMessageBox>
#include <QLocale>

int main(int argc, char *argv[])
{
	int ret = 0;

	QScopedPointer<qf::core::LogDevice> file_log_device(qf::core::FileLogDevice::install());
	file_log_device->setDomainTresholds(argc, argv);
	file_log_device->setPrettyDomain(true);

	qfError() << "qfError() test OK.";
	qfWarning() << "qfWarning() test OK.";
	qfInfo() << "qfInfo() test OK.";
	qfDebug() << "qfDebug() test OK.";

	//QLocale::setDefault(QLocale(QLocale::Czech, QLocale::CzechRepublic));
	//QLocale::setDefault(QLocale::system());
	/// defaultni locales jsou z LC_ALL
	qfInfo() << "Default locale set to:" << QLocale().name();

	/*
	qfTrash() << "c1";
	QFSqlConnection c1;
	qfTrash() << "c1.refCnt():" << c1.refCnt();
	qfTrash() << "c2(c1)";
	QFSqlConnection c2(c1);
	qfTrash() << "c1.refCnt():" << c1.refCnt();
	qfTrash() << "c2.refCnt():" << c2.refCnt();
	qfTrash() << "c3";
	QFSqlConnection c3;
	qfTrash() << "c3 = c2";
	c3 = c2;
	qfTrash() << "c3.refCnt():" << c3.refCnt();
	//qfTrash() << "c4";
	//QFSqlConnection c4;
	return 0;
	*/
	TheApp app(argc, argv); // have to be outside try block, because of QMessageBox

	MainWindow main_window;
	main_window.show();
	ret = app.exec();

	qfDebug() << "bye ...";
	return ret;
}
