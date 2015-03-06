#include "cardreadoutwidget.h"
#include "ui_cardreadoutwidget.h"
#include "dlgsettings.h"

#include <qf/qmlwidgets/action.h>
#include <qf/qmlwidgets/framework/application.h>
#include <qf/qmlwidgets/framework/partwidget.h>
#include <qf/qmlwidgets/framework/mainwindow.h>
#include <qf/qmlwidgets/menubar.h>
#include <qf/qmlwidgets/toolbar.h>

#include <siut/sidevicedriver.h>
#include <siut/simessage.h>

#include <qf/core/log.h>

#include <QSettings>
#include <QFile>
#include <QTextStream>

namespace qff = qf::qmlwidgets::framework;
namespace qfw = qf::qmlwidgets;

const char *CardReadoutWidget::SETTINGS_PREFIX = "plugins/CardReadout";

CardReadoutWidget::CardReadoutWidget(QWidget *parent) :
	Super(parent),
	ui(new Ui::CardReadoutWidget)
{
	ui->setupUi(this);

	createActions();

	qff::Application *app = qff::Application::instance();
	qff::MainWindow *fw = app->frameWork();

	fw->menuBar()->actionForPath("tools/pluginSettings")->addActionInto(m_actSettings);

	/*
	qfw::Action *a = fw->menuBar()->actionForPath("view", false);
	if(!a) {
		qfError() << "View doesn't exist";
	}
	else {
		a = a->addMenuBefore("cards", "&Cards");
		a->addActionInto(m_actCommOpen);
		//a->addActionInto(m_actSqlConnect);
	}

	qfw::ToolBar *main_tb = fw->toolBar("main", true);
	main_tb->addAction(m_actCommOpen);
	*/
	{
		siut::DeviceDriver *drv = siDriver();
		connect(drv, &siut::DeviceDriver::driverInfo, this, &CardReadoutWidget::processDriverInfo, Qt::QueuedConnection);
		connect(drv, &siut::DeviceDriver::messageReady, this, &CardReadoutWidget::processSIMessage, Qt::QueuedConnection);
		connect(drv, &siut::DeviceDriver::rawDataReceived, this, &CardReadoutWidget::processDriverRawData, Qt::QueuedConnection);
		connect(this, &CardReadoutWidget::sendSICommand, drv, &siut::DeviceDriver::sendCommand, Qt::QueuedConnection);
	}
}

CardReadoutWidget::~CardReadoutWidget()
{
	QF_SAFE_DELETE(m_cardLog);
	QF_SAFE_DELETE(m_cardLogFile);
	delete ui;
}

void CardReadoutWidget::settleDownInPartWidget(qf::qmlwidgets::framework::PartWidget *part_widget)
{
	qfw::Action *a = part_widget->menuBar()->actionForPath("station", true);
	a->setText("&Station");
	a->addActionInto(m_actCommOpen);

	qfw::ToolBar *main_tb = part_widget->addToolBar();
	main_tb->addAction(m_actCommOpen);
}

void CardReadoutWidget::createActions()
{
	//QStyle *sty = style();
	{
		QIcon ico(":/quickevent/CardReadout/images/comm");
		qf::qmlwidgets::Action *a = new qf::qmlwidgets::Action(ico, tr("Open COM"), this);
		a->setCheckable(true);
		connect(a, SIGNAL(triggered(bool)), this, SLOT(onCommOpen(bool)));
		m_actCommOpen = a;
	}
	{
		qf::qmlwidgets::Action *a = new qf::qmlwidgets::Action(tr("Card readout settings"), this);
		connect(a, SIGNAL(triggered()), this, SLOT(openSettings()));
		m_actSettings = a;
	}
	/*
	{
		QIcon ico(":/quickevent/CardReadout/images/sql");
		qf::qmlwidgets::Action *a = new qf::qmlwidgets::Action(ico, tr("Connect SQL"), this);
		a->setCheckable(true);
		connect(a, SIGNAL(triggered(bool)), this, SLOT(sqlConnect(bool)));
		m_actSqlConnect = a;
	}
	*/
}

void CardReadoutWidget::openSettings()
{
	DlgSettings dlg(this);
	if(dlg.exec()) {
		closeCardLog();
	}
}

qf::core::Log::Level CardReadoutWidget::logLevelFromSettings()
{
	QSettings settings;
	QString key = QString(SETTINGS_PREFIX) + "/logging/level";
	QString level_str = settings.value(key).toString().toLower();
	//if(level_str == "trash") return qf::core::Log::LOG_TRASH;
	if(level_str == "debug")
		return qf::core::Log::LOG_DEB;
	if(level_str == "info")
		return qf::core::Log::LOG_INFO;
	if(level_str == "warning")
		return qf::core::Log::LOG_WARN;
	if(level_str == "error")
		return qf::core::Log::LOG_ERR;
	return qf::core::Log::LOG_INFO;
}

QTextStream& CardReadoutWidget::cardLog()
{
	if(!m_cardLog) {
		QSettings settings;
		QString key = QString(SETTINGS_PREFIX) + "/logging/cardLog";
		QString fn = settings.value(key).toString();
		if(!fn.isEmpty()) {
			QF_SAFE_DELETE(m_cardLogFile);
			m_cardLogFile = new QFile(fn);
			if(m_cardLogFile->open(QFile::Append)) {
				emitLogRequest(qf::core::Log::LOG_INFO, tr("Openned card log file '%1' for append.").arg(fn));
				m_cardLog = new QTextStream(m_cardLogFile);
			}
			else {
				emitLogRequest(qf::core::Log::LOG_ERR, tr("Can't open card log file '%1' for append.").arg(fn));
			}
		}
		if(!m_cardLog) {
			/// log to stdout
			m_cardLog = new QTextStream(stdout);
			emitLogRequest(qf::core::Log::LOG_INFO, tr("Card log file redirected to stdout."));
		}
	}
	return *m_cardLog;
}

void CardReadoutWidget::closeCardLog()
{
	emitLogRequest(qf::core::Log::LOG_INFO, tr("Closing card log."));
	QF_SAFE_DELETE(m_cardLog);
}

siut::DeviceDriver *CardReadoutWidget::siDriver()
{
	if(!f_siDriver) {
		f_siDriver = new siut::DeviceDriver(this);
		//connect(f_siDriver, SIGNAL(messageReady(const SIMessageBase&)), this, SLOT(onMessageReady(const SIMessageBase&)));
	}
	return f_siDriver;
}

void CardReadoutWidget::onCommOpen(bool checked)
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
		siDriver()->openCommPort(device, baud_rate, data_bits, parity, stop_bits > 1);
		//theApp()->scriptDriver()->callExtensionFunction("onCommConnect", QVariantList() << device);
	}
	else {
		siDriver()->closeCommPort();
	}
}

void CardReadoutWidget::appendLog(int level, const QString& msg)
{
	qf::core::Log::Level treshold = logLevelFromSettings();
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

void CardReadoutWidget::appendLogPre(int level, const QString& msg)
{
	appendLog(level, "<pre>" + msg + "</pre>");
}

void CardReadoutWidget::processSIMessage(const SIMessageData& msg_data)
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

void CardReadoutWidget::processDriverInfo ( int level, const QString& msg )
{
	qfLogFuncFrame() << level << msg;
	appendLog(level, trUtf8("DriverInfo: <%1> %2").arg(qf::core::Log::levelName((qf::core::Log::Level)level)).arg(msg));
}

void CardReadoutWidget::processDriverRawData(const QByteArray& data)
{
	QSettings settings;
	if(settings.value("comm/debug/showRawComData").toBool()) {
		QString msg = SIMessageData::dumpData(data);
		appendLog(qf::core::Log::LOG_DEB, trUtf8("DriverRawData: %1").arg(msg));
	}
}

