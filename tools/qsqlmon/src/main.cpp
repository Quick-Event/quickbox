#include "theapp.h"
#include "mainwindow.h"
#include "appversion.h"

#include <qf/core/log.h>

//#include <QDialog>
#include <Qt>
#include <QMessageBox>
#include <QLocale>

int main(int argc, char *argv[])
{
	int ret = 0;

	QCoreApplication::setOrganizationDomain("quickbox.org");
	QCoreApplication::setOrganizationName("quickbox");
	QCoreApplication::setApplicationName("qsqlmon");
	QCoreApplication::setApplicationVersion(APP_VERSION);

	std::vector<std::string> shv_args = NecroLog::setCLIOptions(argc, argv);
	QStringList args;
	for(const auto &s : shv_args)
		args << QString::fromStdString(s);

	qfInfo() << QCoreApplication::applicationName() << "ver." << QCoreApplication::applicationVersion();

	TheApp app(argc, argv);

	MainWindow main_window;
	main_window.show();
	ret = app.exec();

	qfInfo() << "bye ...";
	return ret;
}
