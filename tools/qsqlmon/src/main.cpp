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

	QCoreApplication::setOrganizationDomain("quickbox.org");
	QCoreApplication::setOrganizationName("QuickBox");
	QCoreApplication::setApplicationName("qsqlmon");

	qf::core::LogDevice::setGlobalTresholds(argc, argv);
	QScopedPointer<qf::core::LogDevice> file_log_device(qf::core::FileLogDevice::install());

	qfError() << "qfError() test OK.";
	qfWarning() << "qfWarning() test OK.";
	qfInfo() << "qfInfo() test OK.";
	qfDebug() << "qfDebug() test OK.";

	//QLocale::setDefault(QLocale(QLocale::Czech, QLocale::CzechRepublic));
	//QLocale::setDefault(QLocale::system());
	/// defaultni locales jsou z LC_ALL
	qfInfo() << "Default locale set to:" << QLocale().name();

	TheApp app(argc, argv);

	MainWindow main_window;
	main_window.show();
	ret = app.exec();

	qfDebug() << "bye ...";
	return ret;
}
