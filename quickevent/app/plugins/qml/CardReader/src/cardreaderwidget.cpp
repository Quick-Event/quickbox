#include "cardreaderwidget.h"
#include "ui_cardreaderwidget.h"
#include "dlgsettings.h"
#include "cardreaderpartwidget.h"
#include "CardReader/cardreaderplugin.h"
#include "CardReader/cardchecker.h"
#include "CardReader/readcard.h"
#include "CardReader/checkedcard.h"

#include <Event/eventplugin.h>

#include <quickevent/og/timems.h>
#include <quickevent/og/sqltablemodel.h>
#include <quickevent/og/itemdelegate.h>

#include <siut/sidevicedriver.h>
#include <siut/simessage.h>

#include <qf/qmlwidgets/action.h>
#include <qf/qmlwidgets/framework/application.h>
#include <qf/qmlwidgets/framework/partwidget.h>
#include <qf/qmlwidgets/framework/mainwindow.h>
#include <qf/qmlwidgets/menubar.h>
#include <qf/qmlwidgets/toolbar.h>

#include <qf/core/log.h>
#include <qf/core/exception.h>
#include <qf/core/sql/query.h>
#include <qf/core/sql/dbenum.h>
#include <qf/core/model/sqltablemodel.h>

#include <QSettings>
#include <QFile>
#include <QTextStream>
#include <QComboBox>
#include <QLabel>

namespace qfm = qf::core::model;
namespace qfs = qf::core::sql;
namespace qff = qf::qmlwidgets::framework;
namespace qfw = qf::qmlwidgets;

const char *CardReaderWidget::SETTINGS_PREFIX = "plugins/CardReader";

CardReaderWidget::CardReaderWidget(QWidget *parent) :
	Super(parent),
	ui(new Ui::CardReaderWidget)
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
		connect(drv, &siut::DeviceDriver::driverInfo, this, &CardReaderWidget::processDriverInfo, Qt::QueuedConnection);
		connect(drv, &siut::DeviceDriver::messageReady, this, &CardReaderWidget::processSIMessage, Qt::QueuedConnection);
		connect(drv, &siut::DeviceDriver::rawDataReceived, this, &CardReaderWidget::processDriverRawData, Qt::QueuedConnection);
		connect(this, &CardReaderWidget::sendSICommand, drv, &siut::DeviceDriver::sendCommand, Qt::QueuedConnection);
	}
	{
		ui->tblCardsTB->setTableView(ui->tblCards);

		ui->tblCards->setPersistentSettingsId("tblCards");
		ui->tblCards->setRowEditorMode(qfw::TableView::EditRowsMixed);
		ui->tblCards->setInlineEditStrategy(qfw::TableView::OnCurrentFieldChange);
		ui->tblCards->setItemDelegate(new quickevent::og::ItemDelegate(ui->tblCards));
		auto m = new quickevent::og::SqlTableModel(this);
		m->addColumn("cards.id", "ID").setReadOnly(true);
		m->addColumn("cards.siId", tr("SI")).setReadOnly(true);
		m->addColumn("classes.name", tr("Class"));
		m->addColumn("competitorName", tr("Name"));
		m->addColumn("competitors.registration", tr("Reg"));
		m->addColumn("runs.startTimeMs", tr("Start")).setCastType(qMetaTypeId<quickevent::og::TimeMs>());
		m->addColumn("runs.timeMs", tr("Time")).setCastType(qMetaTypeId<quickevent::og::TimeMs>());
		qfm::SqlTableModel::ColumnDefinition::DbEnumCastProperties status_props;
		status_props.setGroupName("runs.status");
		m->addColumn("runs.status", tr("Status"))
				.setCastType(qMetaTypeId<qf::core::sql::DbEnum>())
				.setCastProperties(status_props);
		ui->tblCards->setTableModel(m);
		m_cardsModel = m;
	}
	ui->tblCards->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(ui->tblCards, &qfw::TableView::customContextMenuRequested, this, &CardReaderWidget::onCustomContextMenuRequest);
}

void CardReaderWidget::onCustomContextMenuRequest(const QPoint & pos)
{
	qfLogFuncFrame();
	QAction a_show_card(tr("Show card"), nullptr);
	QList<QAction*> lst;
	lst << &a_show_card;
	QAction *a = QMenu::exec(lst, ui->tblCards->viewport()->mapToGlobal(pos));
	if(a == &a_show_card) {
		showSelectedCard();
	}
}

CardReaderWidget::~CardReaderWidget()
{
	QF_SAFE_DELETE(m_cardLog);
	QF_SAFE_DELETE(m_cardLogFile);
	delete ui;
}

void CardReaderWidget::settleDownInPartWidget(CardReaderPartWidget *part_widget)
{
	connect(part_widget, SIGNAL(resetPartRequest()), this, SLOT(reset()));
	connect(part_widget, SIGNAL(reloadPartRequest()), this, SLOT(reset()));

	qfw::Action *a = part_widget->menuBar()->actionForPath("station", true);
	a->setText("&Station");
	a->addActionInto(m_actCommOpen);

	qfw::ToolBar *main_tb = part_widget->toolBar("main", true);
	main_tb->addAction(m_actCommOpen);
	{
		QLabel *lbl = new QLabel(" Check type ");
		main_tb->addWidget(lbl);
		auto *card_reader_plugin = qobject_cast<CardReader::CardReaderPlugin*>(part_widget->plugin(qf::core::Exception::Throw));
		m_cbxCardCheckers = new QComboBox();
		for(auto checker : card_reader_plugin->cardCheckers()) {
			m_cbxCardCheckers->addItem(checker->caption());
		}
		main_tb->addWidget(m_cbxCardCheckers);
		connect(m_cbxCardCheckers, SIGNAL(activated(int)), this, SLOT(onCbxCardCheckersActivated(int)));
		onCbxCardCheckersActivated(m_cbxCardCheckers->currentIndex());
	}
}

void CardReaderWidget::reload()
{
	int current_stage = thisPlugin()->currentStageId();
	qfs::QueryBuilder qb;
	qb.select2("cards", "id, siId")
			.select2("runs", "startTimeMs, timeMs, status")
			.select2("competitors", "registration")
			.select2("classes", "name")
			.select("COALESCE(lastName, '') || ' ' || COALESCE(firstName, '') AS competitorName")
			.from("cards")
			.join("cards.runId", "runs.id")
			.join("runs.competitorId", "competitors.id")
			.join("competitors.classId", "classes.id")
			.where("cards.stageId=" QF_IARG(current_stage))
			.orderBy("cards.id DESC");
	m_cardsModel->setQueryBuilder(qb);
	m_cardsModel->reload();
}

void CardReaderWidget::createActions()
{
	//QStyle *sty = style();
	{
		QIcon ico(":/quickevent/CardReader/images/comm");
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
		QIcon ico(":/quickevent/CardReader/images/sql");
		qf::qmlwidgets::Action *a = new qf::qmlwidgets::Action(ico, tr("Connect SQL"), this);
		a->setCheckable(true);
		connect(a, SIGNAL(triggered(bool)), this, SLOT(sqlConnect(bool)));
		m_actSqlConnect = a;
	}
	*/
}

void CardReaderWidget::openSettings()
{
	DlgSettings dlg(this);
	if(dlg.exec()) {
		closeCardLog();
	}
}

qf::core::Log::Level CardReaderWidget::logLevelFromSettings()
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

QTextStream& CardReaderWidget::cardLog()
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

void CardReaderWidget::closeCardLog()
{
	emitLogRequest(qf::core::Log::LOG_INFO, tr("Closing card log."));
	QF_SAFE_DELETE(m_cardLog);
}

siut::DeviceDriver *CardReaderWidget::siDriver()
{
	if(!f_siDriver) {
		f_siDriver = new siut::DeviceDriver(this);
		//connect(f_siDriver, SIGNAL(messageReady(const SIMessageBase&)), this, SLOT(onMessageReady(const SIMessageBase&)));
	}
	return f_siDriver;
}

void CardReaderWidget::onCommOpen(bool checked)
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

void CardReaderWidget::appendLog(int level, const QString& msg)
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

void CardReaderWidget::appendLogPre(int level, const QString& msg)
{
	appendLog(level, "<pre>" + msg + "</pre>");
}

void CardReaderWidget::processSIMessage(const SIMessageData& msg_data)
{
	qfLogFuncFrame();
	//appendLog(qf::core::Log::LOG_INFO, trUtf8("processSIMessage command: %1 , type: %2").arg(SIMessageData::commandName(msg_data.command())).arg(msg_data.type()));
	if(msg_data.type() == SIMessageData::MsgCardReadOut) {
		SIMessageCardReadOut card(msg_data);
		processSICard(card);
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

void CardReaderWidget::processDriverInfo ( int level, const QString& msg )
{
	qfLogFuncFrame() << level << msg;
	appendLog(level, trUtf8("DriverInfo: <%1> %2").arg(qf::core::Log::levelName((qf::core::Log::Level)level)).arg(msg));
}

void CardReaderWidget::processDriverRawData(const QByteArray& data)
{
	QSettings settings;
	if(settings.value("comm/debug/showRawComData").toBool()) {
		QString msg = SIMessageData::dumpData(data);
		appendLog(qf::core::Log::LOG_DEB, trUtf8("DriverRawData: %1").arg(msg));
	}
}

void CardReaderWidget::processSICard(const SIMessageCardReadOut &card)
{
	appendLogPre(qf::core::Log::LOG_DEB, card.dump());
	appendLog(qf::core::Log::LOG_INFO, trUtf8("card: %1").arg(card.cardNumber()));
	int run_id = thisPlugin()->findRunId(card.cardNumber());
	if(run_id == 0)
		appendLog(qf::core::Log::LOG_ERR, trUtf8("Cannot find run for SI: %1").arg(card.cardNumber()));
	CardReader::ReadCard read_card(card);
	read_card.setRunId(run_id);
	int card_id = thisPlugin()->saveCardToSql(read_card);
	if(run_id) {
		CardReader::CheckedCard checked_card = thisPlugin()->checkCard(read_card);
		thisPlugin()->updateRunLapsSql(checked_card);
	}
	if(card_id > 0)
		updateTableView(card_id);
}

void CardReaderWidget::updateTableView(int card_id)
{
	if(card_id <= 0)
		return;
	m_cardsModel->insertRow(0);
	m_cardsModel->setValue(0, QStringLiteral("cards.id"), card_id);
	int reloaded_row_cnt = m_cardsModel->reloadRow(0);
	if(reloaded_row_cnt != 1) {
		qfWarning() << "Inserted/Copied row id:" << card_id << "reloaded in" << reloaded_row_cnt << "instances.";
		return;
	}
	ui->tblCards->updateRow(0);
}

CardReader::CardReaderPlugin *CardReaderWidget::thisPlugin()
{
	qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
	auto cardreader_plugin = qobject_cast<CardReader::CardReaderPlugin *>(fwk->plugin("CardReader"));
	QF_ASSERT_EX(cardreader_plugin != nullptr, "Bad plugin");
	return cardreader_plugin;
}

qf::qmlwidgets::framework::Plugin *CardReaderWidget::receipesPlugin()
{
	qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
	auto plugin = qobject_cast<qf::qmlwidgets::framework::Plugin *>(fwk->plugin("Receipes"));
	QF_ASSERT(plugin != nullptr, "Bad plugin", return nullptr);
	return plugin;
}

void CardReaderWidget::onCbxCardCheckersActivated(int ix)
{
	thisPlugin()->setCurrentCardCheckerIndex(ix);
}

void CardReaderWidget::showSelectedCard()
{
	qfLogFuncFrame();
	auto receipes_plugin = receipesPlugin();
	if(!receipes_plugin)
		return;
	int card_id = ui->tblCards->selectedRow().value("cards.id").toInt();
	QMetaObject::invokeMethod(receipes_plugin, "previewReceipe", Q_ARG(int, card_id));
}

