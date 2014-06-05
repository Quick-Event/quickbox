#include "ui_mainwindow.h"
#include "mainwindow.h"
#include "dlgsettings.h"

#include "theapp.h"

#include <siut/sidevicedriver.h>
#include <siut/simessage.h>

#include <qf/core/log.h>

#include <QSettings>
#include <QMessageBox>

//#include "sicliscriptdriver.h"

MainWindow::MainWindow(QWidget * parent, Qt::WindowFlags flags)
: QMainWindow(parent, flags), ui(new Ui::MainWindow)
{
	//qDebug() << "setup ui";
	ui->setupUi(this);
	setWindowTitle("QSIClient");
	//qDebug() << "setup ui OK";
	connect(theApp(), SIGNAL(logRequest(int,QString)), this, SLOT(appendLog(int,QString)));
	//connect(theApp(), SIGNAL(logRequestPre(int,QString)), this, SLOT(appendLogPre(int,QString)));

	connect(ui->actCommOpen, &QAction::toggled, this, &MainWindow::onCommOpen);
	connect(ui->actSqlConnect, &QAction::toggled, theApp(), &TheApp::connectSql);
	connect(ui->actConfig, &QAction::triggered, this, &MainWindow::actConfigTriggered);
	connect(ui->actHelpAbout, &QAction::triggered, this, &MainWindow::onHelpAbout);
	connect(ui->actHelpAboutQt, &QAction::triggered, this, &MainWindow::onHelpAboutQt);
	{
		siut::DeviceDriver *drv = theApp()->siDriver();
		connect(drv, &siut::DeviceDriver::driverInfo, this, &MainWindow::processDriverInfo, Qt::QueuedConnection);
		connect(drv, &siut::DeviceDriver::messageReady, this, &MainWindow::processSIMessage, Qt::QueuedConnection);
		connect(drv, &siut::DeviceDriver::rawDataReceived, this, &MainWindow::processDriverRawData, Qt::QueuedConnection);
		connect(this, &MainWindow::sendSICommand, drv, &siut::DeviceDriver::sendCommand, Qt::QueuedConnection);
	}
	{
		QSettings settings;
		QRect r = settings.value("mainWindow/geometry").toRect();
		if(r.isValid()) setGeometry(r);
	}
}

MainWindow::~MainWindow()
{
	{
		QSettings settings;
		QRect r = geometry();
		settings.setValue("mainWindow/geometry", r);
	}
	delete  ui;
}

void MainWindow::onCommOpen(bool checked)
{
	qfLogFuncFrame() << "checked:" << checked;
	if(checked) {
		QSettings settings;
		settings.beginGroup("comm");
		settings.beginGroup("connection");
		QString device = settings.value("device", "/dev/ttyUSB0").toString();
		int baud_rate = settings.value("baudRate", 38400).toInt();
		int data_bits = settings.value("dataBits", 8).toInt();
		int stop_bits = settings.value("stopBits", 1).toInt();
		QString parity = settings.value("parity", "none").toString();
		theApp()->siDriver()->openCommPort(device, baud_rate, data_bits, parity, stop_bits > 1);
		//theApp()->scriptDriver()->callExtensionFunction("onCommConnect", QVariantList() << device);
	}
	else theApp()->siDriver()->closeCommPort();
}
/*
void MainWindow::onSqlConnect(bool checked)
{
	qfLogFuncFrame() << "checked:" << checked;
	if(checked) {
		QSettings settings;
		settings.beginGroup("sql");
		settings.beginGroup("connection");
		QString host = settings.value("host", "localhost").toString();
		int port = settings.value("port", 0).toInt();
		QString user = settings.value("user").toString();
		QString password = settings.value("password").toString();
		QString database = settings.value("database").toString();
		QSqlDatabase db = theApp()->sqlConnection();
		db.setHostName(host);
		db.setPort(port);
		db.setUserName(user);
		db.setPassword(password);
		db.setDatabaseName(database);
		appendLog(qf::core::Log::LOG_INFO, tr("Opening database connection: %1@%2:%3/%4 [%5]").arg(user).arg(host).arg(port).arg(database).arg(db.driverName()));
		if(db.open()) {
			appendLog(qf::core::Log::LOG_INFO, tr("OK"));
			//theApp()->scriptDriver()->callExtensionFunction("onSQLConnect");
		}
		else {
			QSqlError err = db.lastError();
			appendLog(qf::core::Log::LOG_ERR, tr("ERROR - %1").arg(err.text()));
		}
	}
	else {
		appendLog(qf::core::Log::LOG_INFO, tr("Closing database connection"));
		theApp()->sqlConnection().close();
		appendLog(qf::core::Log::LOG_INFO, theApp()->sqlConnection().isOpen()? "ERROR": "OK");
		if(theApp()->sqlConnection().isOpen()) {
			QSqlError err = theApp()->sqlConnection().lastError();
			appendLog(qf::core::Log::LOG_ERR, err.text());
		}
	}
}
*/
void MainWindow::appendLog(int level, const QString& msg)
{
	qf::core::Log::Level treshold = theApp()->logLevelFromSettings();
	if(level <= (int)treshold) {
		qfLog(level) << msg;
		QString div = "<div style=\"color:%1\">%2</div>";
		QString color_name;
		switch((qf::core::Log::Level)level) {
			case qf::core::Log::LOG_ERR: color_name = "red"; break;
			case qf::core::Log::LOG_WARN: color_name = "blue"; break;
			case qf::core::Log::LOG_INFO: color_name = "black"; break;
			case qf::core::Log::LOG_DEB: color_name = "darkgray"; break;
			default: color_name = "darkgray"; break;
		}
		ui->txtLog->insertHtml(div.arg(color_name).arg(msg));
		ui->txtLog->append(QString());
		ui->txtLog->textCursor().movePosition(QTextCursor::End);
		ui->txtLog->ensureCursorVisible();
	}
}

void MainWindow::appendLogPre(int level, const QString& msg)
{
	appendLog(level, "<pre>" + msg + "</pre>");
}

void MainWindow::processSIMessage(const SIMessageData& msg_data)
{
	qfLogFuncFrame();
	//appendLog(qf::core::Log::LOG_INFO, trUtf8("processSIMessage command: %1 , type: %2").arg(SIMessageData::commandName(msg_data.command())).arg(msg_data.type()));
	if(msg_data.type() == SIMessageData::MsgCardReadOut) {
		SIMessageCardReadOut msg(msg_data);
		appendLogPre(qf::core::Log::LOG_DEB, msg.dump());
		appendLog(qf::core::Log::LOG_INFO, trUtf8("card: %1").arg(msg.cardNumber()));
		/*
		SICliScriptDriver *drv = theApp()->scriptDriver();
		try {
			QVariantList args;
			args << msg.toVariant();
			QScriptValue sv = drv->callExtensionFunction("onCardReadOut", args);
			QString ret_type = sv.property("retType").toString();
			if(ret_type == "error") {
				QString err = sv.property("error").toString();
				/// tady by to melo zacit vriskat
				appendLog(qf::core::Log::LOG_ERR, sv.property("message").toString());
			}
			else if(ret_type == "info") {
				//QString info = sv.property("info").toString();
				appendLog(qf::core::Log::LOG_INFO, sv.property("message").toString());
			}
		}
		catch(std::exception &e) {
			appendLog(qf::core::Log::LOG_ERR, e.what());
		}
		catch(...) {
			appendLog(qf::core::Log::LOG_ERR, tr("Unknown exception"));
		}
		*/
	}
	else if(msg_data.type() == SIMessageData::MsgCardEvent) {
		appendLogPre(qf::core::Log::LOG_DEB, msg_data.dump());
		if(msg_data.command() == SIMessageData::CmdSICard5DetectedExt) {
			QByteArray data(1, 0);
			data[0] = 0;
			emit sendSICommand(SIMessageData::CmdGetSICard5Ext, data);
		}
		else if(msg_data.command() == SIMessageData::CmdSICard6DetectedExt) {
			QByteArray data(2, 0);
			data[0] = 1;
			data[1] = 8;
			emit sendSICommand(SIMessageData::CmdGetSICard6Ext, data);
		}
		else if(msg_data.command() == SIMessageData::CmdSICard8AndHigherDetectedExt) {
			QByteArray data(2, 0);
			data[0] = 1;
			data[1] = 8;
			emit sendSICommand(SIMessageData::CmdGetSICard8Ext, data);
		}
	}
	else {
		appendLogPre(qf::core::Log::LOG_DEB, msg_data.dump());
	}
}

void MainWindow::processDriverInfo ( int level, const QString& msg )
{
	appendLog(level, trUtf8("DriverInfo: <%1> %2").arg(qf::core::Log::levelName((qf::core::Log::Level)level)).arg(msg));
}

void MainWindow::processDriverRawData(const QByteArray& data)
{
	QSettings settings;
	if(settings.value("comm/debug/showRawComData").toBool()) {
		QString msg = SIMessageData::dumpData(data);
		appendLog(qf::core::Log::LOG_DEB, trUtf8("DriverRawData: %1").arg(msg));
	}
}

void MainWindow::actConfigTriggered()
{
	DlgSettings dlg(this);
	if(dlg.exec()) {
		//qf::core::Log::setDefaultLogTreshold(theApp()->logLevelFromSettings());
	}
}

void MainWindow::onHelpAbout()
{
	QString s = tr("<p><b>SportIdent Client</b></p>"
	"<p>version: %1</p>")
	.arg(theApp()->versionString());
	QMessageBox::about(this, tr("About"), s);
}

void MainWindow::onHelpAboutQt()
{
	QMessageBox::aboutQt(this, tr("About Qt"));
}
