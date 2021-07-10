#include "cardreaderwidget.h"
#include "ui_cardreaderwidget.h"
#include "dlgsettings.h"

#include "cardreaderplugin.h"
#include "cardchecker.h"
#include "cardreaderplugin.h"

#include <quickevent/gui/og/itemdelegate.h>
#include <quickevent/gui/audio/player.h>

#include <quickevent/core/codedef.h>
#include <quickevent/core/og/timems.h>
#include <quickevent/core/og/sqltablemodel.h>
#include <quickevent/core/si/punchrecord.h>
#include <quickevent/core/si/readcard.h>
#include <quickevent/core/si/checkedcard.h>
#include <quickevent/core/si/siid.h>

#include <siut/sidevicedriver.h>
#include <siut/commport.h>
#include <siut/sicard.h>
#include <siut/sitask.h>

#include <qf/qmlwidgets/action.h>
#include <qf/qmlwidgets/framework/application.h>
#include <qf/qmlwidgets/framework/partwidget.h>
#include <qf/qmlwidgets/framework/mainwindow.h>
#include <qf/qmlwidgets/combobox.h>
#include <qf/qmlwidgets/dialogbuttonbox.h>
#include <qf/qmlwidgets/htmlviewwidget.h>
#include <qf/qmlwidgets/menubar.h>
#include <qf/qmlwidgets/toolbar.h>
#include <qf/qmlwidgets/dialogs/dialog.h>
#include <qf/qmlwidgets/dialogs/messagebox.h>
#include <qf/qmlwidgets/dialogs/filedialog.h>
#include <qf/qmlwidgets/dialogs/messagebox.h>
#include <qf/qmlwidgets/log.h>

#include <qf/core/assert.h>
#include <qf/core/exception.h>
#include <qf/core/sql/query.h>
#include <qf/core/sql/dbenum.h>
#include <qf/core/sql/transaction.h>
#include <qf/core/model/sqltablemodel.h>
#include <qf/core/utils/settings.h>
#include <qf/core/utils/csvreader.h>
#include <qf/core/utils/htmlutils.h>
#include <plugins/Receipts/src/receiptsplugin.h>
#include <plugins/Event/src/eventplugin.h>
#include <plugins/Runs/src/findrunnerwidget.h>
#include <plugins/Competitors/src/competitorsplugin.h>

#include <QSettings>
#include <QFile>
#include <QTextStream>
#include <QComboBox>
#include <QLabel>
#include <QCheckBox>
#include <QPushButton>
#include <QProgressDialog>
#include <QTimer>
#include <QSerialPortInfo>

namespace qfc = qf::core;
namespace qfm = qf::core::model;
namespace qfs = qf::core::sql;
namespace qff = qf::qmlwidgets::framework;
namespace qfw = qf::qmlwidgets;
namespace qfd = qf::qmlwidgets::dialogs;
using qf::qmlwidgets::framework::getPlugin;
using Event::EventPlugin;
using CardReader::CardReaderPlugin;
using Receipts::ReceiptsPlugin;
using Competitors::CompetitorsPlugin;

namespace {
class Model : public quickevent::core::og::SqlTableModel
{
	Q_OBJECT
private:
	typedef quickevent::core::og::SqlTableModel Super;
public:
	enum Columns {
		col_cards_id = 0,
		col_cards_siId,
		col_classes_name,
		col_competitorName,
		col_competitors_registration,
		col_runs_startTimeMs,
		col_runs_timeMs,
		col_runs_finishTimeMs,
		col_runs_misPunch,
		col_runs_disqualified,
		col_runs_cardLent,
		col_runs_cardReturned,
		col_cards_checkTime,
		col_cards_startTime,
		col_cards_finishTime,
		col_cards_runIdAssignError,
		col_COUNT
	};
public:
	explicit Model(QObject *parent);

	QVariant data(const QModelIndex &index, int role) const Q_DECL_OVERRIDE;
};

Model::Model(QObject *parent)
	: Super(parent)
{
	clearColumns(col_COUNT);
	setColumn(col_cards_id, ColumnDefinition("cards.id", "ID").setReadOnly(true));
	setColumn(col_cards_siId, ColumnDefinition("cards.siId", tr("SI")).setReadOnly(true).setCastType(qMetaTypeId<quickevent::core::si::SiId>()));
	setColumn(col_classes_name, ColumnDefinition("classes.name", tr("Class")));
	setColumn(col_competitorName, ColumnDefinition("competitorName", tr("Name")));
	setColumn(col_competitors_registration, ColumnDefinition("competitors.registration", tr("Reg")));
	setColumn(col_runs_startTimeMs, ColumnDefinition("runs.startTimeMs", tr("Start")).setCastType(qMetaTypeId<quickevent::core::og::TimeMs>()).setReadOnly(true));
	setColumn(col_runs_timeMs, ColumnDefinition("runs.timeMs", tr("Time")).setCastType(qMetaTypeId<quickevent::core::og::TimeMs>()).setReadOnly(true));
	setColumn(col_runs_finishTimeMs, ColumnDefinition("runs.finishTimeMs", tr("Finish")).setCastType(qMetaTypeId<quickevent::core::og::TimeMs>()).setReadOnly(true));
	setColumn(col_runs_misPunch, ColumnDefinition("runs.misPunch", tr("Error")).setToolTip(tr("Card mispunch")).setReadOnly(true));
	setColumn(col_runs_disqualified, ColumnDefinition("runs.disqualified", tr("DISQ")).setToolTip(tr("Disqualified")));
	setColumn(col_runs_cardLent, ColumnDefinition("cardLent", tr("RT")).setToolTip(tr("Card in rent table")).setReadOnly(true).setCastType(QVariant::Bool));
	setColumn(col_runs_cardReturned, ColumnDefinition("runs.cardReturned", tr("R")).setToolTip(tr("Card returned")));
	setColumn(col_cards_checkTime, ColumnDefinition("cards.checkTime", tr("CTIME")).setToolTip(tr("Card check time")).setReadOnly(true));
	setColumn(col_cards_startTime, ColumnDefinition("cards.startTime", tr("STIME")).setToolTip(tr("Card start time")).setReadOnly(true));
	setColumn(col_cards_finishTime, ColumnDefinition("cards.finishTime", tr("FTIME")).setToolTip(tr("Card finish time")).setReadOnly(true));
	setColumn(col_cards_runIdAssignError, ColumnDefinition("cards.runIdAssignError", tr("Error")).setToolTip(tr("Assign card to runner error")).setReadOnly(true));
}

QVariant Model::data(const QModelIndex &index, int role) const
{
	int col = index.column();
	if(role == Qt::BackgroundRole) {
		static auto C_RUNID = QStringLiteral("cards.runId");
		int run_id = tableRow(index.row()).value(C_RUNID).toInt();
		if(run_id == 0) {
			static auto c = QColor(Qt::red).lighter(150);
			return c;
		}
	}
	else if(role == Qt::DisplayRole) {
		if(col == col_cards_checkTime
		   || col == col_cards_startTime
		   || col == col_cards_finishTime )
		{
			int secs = value(index.row(), col).toInt();
			if(secs == 0xEEEE)
				return QString();
			int sec = secs % 60;
			secs /= 60;
			int min = secs % 60;
			secs /= 60;
			int hod = secs;
			QString s("%1:%2:%3");
			s = s.arg(hod, 2, 10, QChar('0'));
			s = s.arg(min, 2, 10, QChar('0'));
			s = s.arg(sec, 2, 10, QChar('0'));
			return s;
		}
	}
	return Super::data(index, role);
}
}

CardReaderWidget::CardReaderWidget(QWidget *parent)
	: Super(parent)
	, ui(new Ui::CardReaderWidget)
{
	ui->setupUi(this);

	ui->tblCards->setInsertRowEnabled(false);
	ui->tblCards->setRemoveRowEnabled(false);
	ui->tblCards->setCloneRowEnabled(false);
	ui->tblCards->setPersistentSettingsId(ui->tblCards->objectName());

	createActions();

	connect(getPlugin<CardReaderPlugin>(), &CardReader::CardReaderPlugin::siTaskFinished, this, &CardReaderWidget::onSiTaskFinished);
	{
		ui->tblCardsTB->setTableView(ui->tblCards);

		ui->tblCards->setPersistentSettingsId("tblCards");
		ui->tblCards->setRowEditorMode(qfw::TableView::EditRowsMixed);
		ui->tblCards->setInlineEditSaveStrategy(qfw::TableView::OnEditedValueCommit);
		ui->tblCards->setItemDelegate(new quickevent::gui::og::ItemDelegate(ui->tblCards));
		auto m = new Model(this);
		ui->tblCards->setTableModel(m);
		m_cardsModel = m;
	}
	ui->tblCards->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(ui->tblCards, &qfw::TableView::customContextMenuRequested, this, &CardReaderWidget::onCustomContextMenuRequest);
	/*
	{
		QTimer *t = new QTimer(this);
		connect(t, &QTimer::timeout, [this]() {
			qfInfo() << "CardReaderWidget visible:" << isVisible();
		});
		t->start(2000);
	}
	*/
}

void CardReaderWidget::onCustomContextMenuRequest(const QPoint & pos)
{
	qfLogFuncFrame();
	QAction a_show_receipt(tr("Show receipt"), nullptr);
	QAction a_print_receipt(tr("Print receipt"), nullptr);
	QAction a_sep1(nullptr); a_sep1.setSeparator(true);
	QAction a_show_card(tr("Show card data"), nullptr);
	QAction a_print_card(tr("Print card data"), nullptr);
	QAction a_sep2(nullptr); a_sep2.setSeparator(true);
	QAction a_assign_runner(tr("Assign card to runner"), nullptr);
	QAction a_recalculate_times(tr("Recalculate times in selected rows"), nullptr);
	QList<QAction*> lst;
	lst << &a_show_receipt << &a_print_receipt
		<< &a_sep1
		<< &a_show_card << &a_print_card
		<< &a_sep2
		<< &a_assign_runner << &a_recalculate_times;
	QAction *a = QMenu::exec(lst, ui->tblCards->viewport()->mapToGlobal(pos));
	if(a == &a_show_receipt) {
		showSelectedReceipt();
	}
	if(a == &a_show_card) {
		showSelectedCard();
	}
	else if(a == &a_print_receipt) {
		int card_id = ui->tblCards->tableRow().value("cards.id").toInt();
		getPlugin<ReceiptsPlugin>()->printReceipt(card_id);
	}
	else if(a == &a_print_card) {
		int card_id = ui->tblCards->tableRow().value("cards.id").toInt();
		getPlugin<ReceiptsPlugin>()->printCard(card_id);
	}
	else if(a == &a_assign_runner) {
		assignRunnerToSelectedCard();
	}
	else if(a == &a_recalculate_times) {
		qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
		int curr_ix = 0;
		QList<int> sel_ixs = ui->tblCards->selectedRowsIndexes();
		for(int ix : sel_ixs) {
			qf::core::utils::TableRow row = ui->tblCards->tableRow(ix);
			int card_id = row.value(QStringLiteral("id")).toInt();
			int run_id = row.value(QStringLiteral("runId")).toInt();
			fwk->showProgress(tr("Recalculating times for %1").arg(row.value(QStringLiteral("competitorName")).toString()), ++curr_ix, sel_ixs.count());
			try {
				qf::core::sql::Transaction transaction;
				getPlugin<CardReaderPlugin>()->reloadTimesFromCard(card_id, run_id, false);
				ui->tblCards->reloadRow(ix);
				transaction.commit();
			}
			catch (const qf::core::Exception &e) {
				qfError() << "Update runs & runlaps ERROR:" << e.message();
			}
		}
		fwk->hideProgress();
	}
}

CardReaderWidget::~CardReaderWidget()
{
	delete ui;
}

void CardReaderWidget::settleDownInPartWidget(quickevent::gui::PartWidget *part_widget)
{
	connect(part_widget, SIGNAL(resetPartRequest()), this, SLOT(reset()));
	connect(part_widget, SIGNAL(reloadPartRequest()), this, SLOT(reload()));
	{
		qfw::Action *a_station = part_widget->menuBar()->actionForPath("station", true);
		a_station->setText(tr("&Station"));
		a_station->addActionInto(m_actSettings);
		//a_station->addActionInto(m_actCommOpen);
		{
			QAction *a = new QAction(tr("Station info"));
			a->setEnabled(false);
			connect(commPort(), &siut::CommPort::openChanged, a, &QAction::setEnabled);
			connect(a, &QAction::triggered, [this]() {
				siut::SiTaskStationConfig *cmd = new siut::SiTaskStationConfig();
				connect(cmd, &siut::SiTaskStationConfig::finished, this, [this](bool ok, QVariant result) {
					if(ok) {
						siut::SiStationConfig cfg(result.toMap());
						QString msg = tr(""
										 "Station number: {{StationNumber}}\n"
										 "Extended mode: {{ExtendedMode}}\n"
										 "Auto send: {{AutoSend}}\n"
										 "Handshake: {{HandShake}}\n"
										 "Password access: {{PasswordAccess}}\n"
										 "Read out after punch: {{ReadOutAfterPunch}}\n"
										 "");
						msg.replace("{{StationNumber}}", QString::number(cfg.stationNumber()));
						msg.replace("{{ExtendedMode}}", (cfg.flags() & (unsigned)siut::SiStationConfig::Flag::ExtendedMode)? tr("True"): tr("False"));
						msg.replace("{{AutoSend}}", (cfg.flags() & (unsigned)siut::SiStationConfig::Flag::AutoSend)? tr("True"): tr("False"));
						msg.replace("{{HandShake}}", (cfg.flags() & (unsigned)siut::SiStationConfig::Flag::HandShake)? tr("True"): tr("False"));
						msg.replace("{{PasswordAccess}}", (cfg.flags() & (unsigned)siut::SiStationConfig::Flag::PasswordAccess)? tr("True"): tr("False"));
						msg.replace("{{ReadOutAfterPunch}}", (cfg.flags() & (unsigned)siut::SiStationConfig::Flag::ReadOutAfterPunch)? tr("True"): tr("False"));
						qf::qmlwidgets::dialogs::MessageBox::showInfo(this, msg);
					}
				}, Qt::QueuedConnection);
				this->siDriver()->setSiTask(cmd);
			});
			a_station->addActionInto(a);
		}
		{
			QAction *a = new QAction(tr("Read station memory"));
			a->setEnabled(false);
			connect(commPort(), &siut::CommPort::openChanged, a, &QAction::setEnabled);
			connect(a, &QAction::triggered, this, &CardReaderWidget::readStationBackupMemory);
			a_station->addActionInto(a);
		}
		/*
		{
			QAction *a = new QAction("Set master mode");
			connect(a, &QAction::triggered, [this]() {
				CmdSetDirectRemoteMode *cmd = new CmdSetDirectRemoteMode(true);
				this->siDriver()->enqueueSiCommand(cmd);
			});
			a_station->addActionInto(a);
		}
		{
			QAction *a = new QAction("Set slave mode");
			connect(a, &QAction::triggered, [this]() {
				CmdSetDirectRemoteMode *cmd = new CmdSetDirectRemoteMode(false);
				this->siDriver()->enqueueSiCommand(cmd);
			});
			a_station->addActionInto(a);
		}
		*/
	}
	{
		qfw::Action *a_tools = part_widget->menuBar()->actionForPath("tools", true);
		a_tools->setText(tr("&Tools"));
		{
			auto *m_import_cards = a_tools->addMenuInto("importCards", tr("Import cards"));
			{
				qfw::Action *a = new qfw::Action(tr("Laps only CSV"));
				connect(a, &qf::qmlwidgets::Action::triggered, this, &CardReaderWidget::importCards_lapsOnlyCsv);
				m_import_cards->addActionInto(a);
			}
			{
				qfw::Action *a = new qfw::Action(tr("SI reader backup memory CSV"));
				connect(a, &qf::qmlwidgets::Action::triggered, this, &CardReaderWidget::importCards_SIReaderBackupMemoryCsv);
				m_import_cards->addActionInto(a);
			}
		}
		{
			qfw::Action *a = new qfw::Action(tr("Test audio"));
			connect(a, &qf::qmlwidgets::Action::triggered, this, &CardReaderWidget::operatorAudioNotify);
			a_tools->addActionInto(a);
		}
	}
	qfw::ToolBar *main_tb = part_widget->toolBar("main", true);
	main_tb->addAction(m_actCommOpen);
	{
		QLabel *lbl = new QLabel(tr(" Check type "));
		main_tb->addWidget(lbl);
		m_cbxCardCheckers = new QComboBox();
		for(auto checker : getPlugin<CardReaderPlugin>()->cardCheckers()) {
			m_cbxCardCheckers->addItem(checker->caption());
		}
		main_tb->addWidget(m_cbxCardCheckers);
		connect(m_cbxCardCheckers, SIGNAL(activated(int)), this, SLOT(onCbxCardCheckersActivated(int)));
		onCbxCardCheckersActivated(m_cbxCardCheckers->currentIndex());
	}
	main_tb->addSeparator();
	{
		QLabel *lbl = new QLabel(tr(" Reader mode "));
		main_tb->addWidget(lbl);
		m_cbxPunchMode = new QComboBox();
		m_cbxPunchMode->addItem(tr("Readout"), PunchMode::Readout);
		m_cbxPunchMode->setItemData(0, tr("Readout mode - default"), Qt::ToolTipRole);
		m_cbxPunchMode->addItem(tr("Edit on punch"), PunchMode::EditOnPunch);
		m_cbxPunchMode->setItemData(1, tr("Show Edit/Insert competitor dialog when SI Card is inserted into the reader station"), Qt::ToolTipRole);
		main_tb->addWidget(m_cbxPunchMode);
	}
	main_tb->addSeparator();
	{
		m_lblCommInfo = new QLabel();
		main_tb->addWidget(m_lblCommInfo);
	}
	connect(getPlugin<EventPlugin>(), &Event::EventPlugin::dbEventNotify, this, &CardReaderWidget::onDbEventNotify, Qt::QueuedConnection);
}

void CardReaderWidget::reset()
{
	if(!getPlugin<EventPlugin>()->isEventOpen()) {
		m_cardsModel->clearRows();
		return;
	}
	reload();
}

void CardReaderWidget::reload()
{
	bool is_relays = getPlugin<EventPlugin>()->eventConfig()->isRelays();
	//QString driver_name = m_cardsModel->sqlConnection().driverName();
	int current_stage = getPlugin<CardReaderPlugin>()->currentStageId();
	qfs::QueryBuilder qb;
	qb.select2("cards", "id, siId, runId, checkTime, startTime, finishTime, runIdAssignError")
			.select2("runs", "id, startTimeMs, timeMs, finishTimeMs, misPunch, disqualified, cardReturned")
			.select2("competitors", "registration")
			.select2("classes", "name")
			.select("COALESCE(lastName, '') || ' ' || COALESCE(firstName, '') AS competitorName")
			.select("lentcards.siid IS NOT NULL OR runs.cardLent AS cardLent")
			.from("cards")
			.joinRestricted("cards.siId", "lentcards.siid", "NOT lentcards.ignored")
			.join("cards.runId", "runs.id")
			.join("runs.competitorId", "competitors.id")
			.where("cards.stageId=" QF_IARG(current_stage))
			.orderBy("cards.id DESC");
	if(is_relays) {
		qb.join("runs.relayId", "relays.id");
		qb.join("relays.classId", "classes.id");
	}
	else {
		qb.join("competitors.classId", "classes.id");
	}
	qfDebug() << qb.toString();
	m_cardsModel->setQueryBuilder(qb, false);
	m_cardsModel->reload();
}

void CardReaderWidget::onDbEventNotify(const QString &domain, int connection_id, const QVariant &data)
{
	Q_UNUSED(connection_id)
	if(domain == QLatin1String(Event::EventPlugin::DBEVENT_CARD_READ)) {
		int card_id = data.toInt();
		if(isVisible())
			updateTableView(card_id);
	}
}

void CardReaderWidget::createActions()
{
	//QStyle *sty = style();
	{
		QIcon ico(":/quickevent/CardReader/images/comm");
		qf::qmlwidgets::Action *a = new qf::qmlwidgets::Action(ico, tr("Open COM"), this);
		a->setCheckable(true);
		connect(a, SIGNAL(triggered(bool)), this, SLOT(onOpenCommTriggered(bool)));
		m_actCommOpen = a;
	}
	{
		qf::qmlwidgets::Action *a = new qf::qmlwidgets::Action(tr("Card reader settings"), this);
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
		//closeCardLog();
	}
}

siut::DeviceDriver *CardReaderWidget::siDriver()
{
	if(!f_siDriver) {
		f_siDriver = new siut::DeviceDriver(this);
		connect(commPort(), &siut::CommPort::readyRead, this, [this]() {
			QByteArray ba = commPort()->readAll();
			siDriver()->processData(ba);
		});
		connect(f_siDriver, &siut::DeviceDriver::dataToSend, commPort(), &siut::CommPort::sendData);
		connect(f_siDriver, &siut::DeviceDriver::siTaskFinished, this, &CardReaderWidget::onSiTaskFinished);
		connect(f_siDriver, &siut::DeviceDriver::driverInfo, this, &CardReaderWidget::processDriverInfo, Qt::QueuedConnection);
	}
	return f_siDriver;
}

siut::CommPort *CardReaderWidget::commPort()
{
	if(!m_commPort) {
		m_commPort = new siut::CommPort(this);
		connect(m_commPort, &siut::CommPort::openChanged, this, &CardReaderWidget::onComOpenChanged);
	}
	return m_commPort;
}

void CardReaderWidget::onComOpenChanged(bool comm_is_open)
{
	if(comm_is_open) {
		siut::SiTaskSetDirectRemoteMode *cmd = new siut::SiTaskSetDirectRemoteMode(siut::SiTaskSetDirectRemoteMode::Mode::Direct);
		connect(cmd, &siut::SiTaskSetDirectRemoteMode::finished, this, [this](bool ok) {
			if(ok) {
				m_lblCommInfo->setText(tr("Connected to %1 in direct mode.").arg(this->commPort()->portName()));
			}
			else {
				m_lblCommInfo->setText(tr("Error set SI station to direct mode."));
			}
		});
		siDriver()->setSiTask(cmd);
	}
	else {
		m_lblCommInfo->setText(QString());
	}
}

void CardReaderWidget::onOpenCommTriggered(bool checked)
{
	qfLogFuncFrame() << "checked:" << checked;
	if(checked) {
		QSettings settings;
		settings.beginGroup(CardReader::CardReaderPlugin::SETTINGS_PREFIX);
		settings.beginGroup("comm");
		settings.beginGroup("connection");
		QString device = settings.value("device", "").toString();
		int baud_rate = settings.value("baudRate", 38400).toInt();
		int data_bits = settings.value("dataBits", 8).toInt();
		int stop_bits = settings.value("stopBits", 1).toInt();
		QString parity = settings.value("parity", "none").toString();
		if(!commPort()->openComm(device, baud_rate, data_bits, parity, stop_bits > 1)) {
			QSerialPort::SerialPortError error_type = commPort()->error();
			QString error_msg = commPort()->errorString();
			if(error_type == QSerialPort::PermissionError) {
				error_msg.append("\n\n")
					 .append(tr(""
						    "possible solution:\n"
						    "Wait at least 10 seconds and then try again."
						    ""));
			}
			if(error_type == QSerialPort::DeviceNotFoundError) {
				error_msg.append("\n\n");
				QList<QSerialPortInfo> port_list = QSerialPortInfo::availablePorts();
				if(port_list.isEmpty()) {
					error_msg.append(tr("There are no ports available."));
				}
				else {
					error_msg.append(tr(""
							    "Selected port %1 is not available.\n"
							    "List of accessible ports:\n\n"
							    "").arg(device));
					for(auto port : port_list) {
						error_msg.append(QChar(0x2022)).append(" ").append(port.systemLocation()).append("\n");
					}
				}
			}
			qf::qmlwidgets::dialogs::MessageBox::showError(this, tr("Error open device %1 - %2").arg(device).arg(error_msg));
		}
		//theApp()->scriptDriver()->callExtensionFunction("onCommConnect", QVariantList() << device);
	}
	else {
		commPort()->closeComm();
	}
}

void CardReaderWidget::appendLog(NecroLog::Level level, const QString& msg)
{
	switch (level) {
	case NecroLog::Level::Debug: qfDebug() << msg; break;
	case NecroLog::Level::Info: qfInfo() << msg; break;
	case NecroLog::Level::Warning: qfWarning() << msg; break;
	default: qfError() << msg; break;
	}
}

void CardReaderWidget::onSiTaskFinished(int task_type, QVariant result)
{
	qfLogFuncFrame();
	siut::SiTask::Type tt = static_cast<siut::SiTask::Type>(task_type);
	if(tt == siut::SiTask::Type::CardRead) {
		siut::SICard card(result.toMap());
		if(card.isEmpty())
			qfError() << "Empty card received";
		else
			processSICard(card);
	}
	else if(tt == siut::SiTask::Type::Punch) {
		siut::SIPunch punch(result.toMap());
		if(punch.isEmpty())
			qfError() << "Empty punch received";
		else
			processSIPunch(punch);
	}
}

/*
void CardReaderWidget::processSIMessage(const SIMessageData& msg_data)
{
	qfLogFuncFrame();
	//appendLog(NecroLog::Level::Info, tr("processSIMessage command: %1 , type: %2").arg(SIMessageData::commandName(msg_data.command())).arg(msg_data.type()));
	if(msg_data.type() == SIMessageData::MessageType::CardReadOut) {
		SIMessageCardReadOut card(msg_data);
		processSICard(card);
	}
	else if(msg_data.type() == SIMessageData::MessageType::CardEvent) {
		appendLog(NecroLog::Level::Debug, msg_data.dump());
		if(msg_data.command() == SIMessageData::Command::SICard5DetectedExt) {
			emit sendSICommand((int)SIMessageData::Command::GetSICard5Ext, QByteArray());
		}
		else if(msg_data.command() == SIMessageData::Command::SICard6DetectedExt) {
			emit sendSICommand((int)SIMessageData::Command::GetSICard6Ext, QByteArray("\x08", 1));
		}
		else if(msg_data.command() == SIMessageData::Command::SICard8AndHigherDetectedExt) {
			emit sendSICommand((int)SIMessageData::Command::GetSICard8Ext, QByteArray("\x08", 1));
		}
	}
	else if(msg_data.type() == SIMessageData::MessageType::Punch) {
		SIMessageTransmitPunch rec(msg_data);
		processSIPunch(rec);
	}
	else {
		appendLog(NecroLog::Level::Debug, msg_data.dump());
	}
}
*/

void CardReaderWidget::processDriverInfo (NecroLog::Level level, const QString& msg )
{
	qf::core::utils::Settings settings;
	if(settings.value(CardReader::CardReaderPlugin::SETTINGS_PREFIX + "/comm/debug/showRawComData").toBool()) {
		if(level == NecroLog::Level::Debug)
			level = NecroLog::Level::Info;
	}
	appendLog(level, tr("DriverInfo: <%1> %2").arg(NecroLog::levelToString((NecroLog::Level)level)).arg(msg));
}

void CardReaderWidget::processDriverRawData(const QByteArray& data)
{
	qf::core::utils::Settings settings;
	//qInfo() << settings.value(CardReader::CardReaderPlugin::SETTINGS_PREFIX + "/comm/debug/showRawComData") << "data:" << data;
	if(settings.value(CardReader::CardReaderPlugin::SETTINGS_PREFIX + "/comm/debug/showRawComData").toBool()) {
		QString msg = siut::SIMessageData::dumpData(data, 16);
		appendLog(NecroLog::Level::Info, tr("DriverRawData: %1").arg(msg));
	}
}

void CardReaderWidget::processSICard(const siut::SICard &card)
{
	appendLog(NecroLog::Level::Debug, card.toString());
	appendLog(NecroLog::Level::Info, tr("card: %1").arg(card.cardNumber()));

	if(currentPunchMode() == PunchMode::EditOnPunch) {
		getPlugin<CompetitorsPlugin>()->editCompetitorOnPunch(card.cardNumber());
		return;
	}

	QString err_msg;
	int run_id = getPlugin<CardReaderPlugin>()->findRunId(card.cardNumber(), card.finishTime(), &err_msg);

	if(run_id == 0) {
		operatorAudioWakeUp();
		appendLog(NecroLog::Level::Error, err_msg);
	}
	else {
		bool card_lent = getPlugin<CardReaderPlugin>()->isCardLent(card.cardNumber(), card.finishTime(), run_id);
		if(card_lent)
			operatorAudioNotify();
	}
	quickevent::core::si::ReadCard read_card(card);
	read_card.setRunId(run_id);
	read_card.setRunIdAssignError(err_msg);
	processReadCardInTransaction(read_card);
}

bool CardReaderWidget::processReadCardInTransaction(const quickevent::core::si::ReadCard &read_card)
{
	try {
		qf::core::sql::Transaction transaction;
		processReadCard(read_card);
		transaction.commit();
		return true;
	}
	catch (qf::core::Exception &e) {
		qfError() << "ERROR processReadCardSafe:" << e.message();
	}
	return false;
}

void CardReaderWidget::processReadCard(const quickevent::core::si::ReadCard &read_card)
{
	int card_id = getPlugin<CardReaderPlugin>()->saveCardToSql(read_card);
	if(card_id && read_card.runId()) {
		getPlugin<CardReaderPlugin>()->assignCardToRun(card_id, read_card.runId());
	}
	if(card_id) {
		/// receipts printer needs this
		/// emitDbEvent is using queued invocation
		getPlugin<EventPlugin>()->emitDbEvent(Event::EventPlugin::DBEVENT_CARD_READ, card_id, true);
	}
}

void CardReaderWidget::processSIPunch(const siut::SIPunch &rec)
{
	appendLog(NecroLog::Level::Info, tr("punch: %1 %2").arg(rec.cardNumber()).arg(rec.code()));
	quickevent::core::si::PunchRecord punch(rec);
	if(currentPunchMode() == PunchMode::Readout) {
		int run_id = getPlugin<CardReaderPlugin>()->findRunId(rec.cardNumber(), 0xEEEE);
		if(run_id == 0)
			appendLog(NecroLog::Level::Error, tr("Cannot find run for punch record SI: %1").arg(rec.cardNumber()));
		else
			punch.setrunid(run_id);
	}
	int punch_id = getPlugin<CardReaderPlugin>()->savePunchRecordToSql(punch);
	if(punch_id > 0) {
		punch.setid(punch_id);
		getPlugin<EventPlugin>()->emitDbEvent(Event::EventPlugin::DBEVENT_PUNCH_RECEIVED, punch, true);
	}
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

void CardReaderWidget::onCbxCardCheckersActivated(int ix)
{
	getPlugin<CardReaderPlugin>()->setCurrentCardCheckerIndex(ix);
}

void CardReaderWidget::showSelectedReceipt()
{
	qfLogFuncFrame();
	int card_id = ui->tblCards->selectedRow().value("cards.id").toInt();
	getPlugin<ReceiptsPlugin>()->previewReceipt(card_id);
}

void CardReaderWidget::showSelectedCard()
{
	qfLogFuncFrame();
	int card_id = ui->tblCards->selectedRow().value("cards.id").toInt();
	getPlugin<ReceiptsPlugin>()->previewCard(card_id);
}

void CardReaderWidget::assignRunnerToSelectedCard()
{
	qfLogFuncFrame();
	int card_id = ui->tblCards->tableRow().value("cards.id").toInt();
	QF_ASSERT(card_id > 0, "Bad card id!", return);
	auto *w = new Runs::FindRunnerWidget(getPlugin<EventPlugin>()->currentStageId());
	w->setWindowTitle(tr("Find runner"));
	qfd::Dialog dlg(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
	dlg.setPersistentSettingsId("dlgAssignRunnerToSelectedCard");
	//dlg.setDefaultButton(QDialogButtonBox::Ok);
	dlg.setCentralWidget(w);
	//w->setFocusToWidget(Runs::FindRunnerWidget::FocusWidget::Name);
	w->focusLineEdit();
	if(dlg.exec()) {
		QVariantMap values = w->selectedRunner();
		//qfDebug() << values;
		int run_id = values.value("runid").toInt();
		int si_id = getPlugin<CardReaderPlugin>()->cardIdToSiId(card_id);
		if(run_id <= 0 || si_id <= 0) {
			return;
		}
		qf::core::sql::QueryBuilder qb;
		qb.select("stageId").select("isRunning").from("runs").where("id=" QF_IARG(run_id) );
		qf::core::sql::Query q;
		q.execThrow(qb.toString());
		if (!q.next()) {
			return;
		}
		int stage_id = q.value("stageId").toInt();
		int competitor_id = values.value("competitorid").toInt();
		{
			bool is_running = q.value("isRunning").toBool();
			if (!is_running) {
				int ret = QMessageBox::warning(this, tr("Competitor off-race"),
								tr("Runner to which you are assinging SI card\n"
								   "is currently flagged \"not running\" for this stage (race).\n"
								   "If you continue, this flag will be removed"),
								QMessageBox::Ok | QMessageBox::Cancel);
				if (ret == QMessageBox::Cancel)
					return;
				QString qs = "UPDATE runs SET isRunning=1 WHERE competitorId=" QF_IARG(competitor_id) " AND stageId=" QF_IARG(stage_id);
				q.execThrow(qs);
			}
		}
		{
			getPlugin<CardReaderPlugin>()->assignCardToRun(card_id, run_id);
			QString qs = "UPDATE runs SET siId=" QF_IARG(si_id) " WHERE competitorId=" QF_IARG(competitor_id) " AND stageId=" QF_IARG(stage_id);
			if(values.value(Runs::FindRunnerWidget::UseSIInNextStages).toBool()) {
				qs = "UPDATE runs SET siId=" QF_IARG(si_id) " WHERE competitorId=" QF_IARG(competitor_id) " AND stageId>=" QF_IARG(stage_id);
			}
			q.execThrow(qs);
		}

		this->ui->tblCards->reloadRow();

		getPlugin<ReceiptsPlugin>()->printOnAutoPrintEnabled(card_id);
	}
}

quickevent::gui::audio::Player *CardReaderWidget::audioPlayer()
{
	if(!m_audioPlayer)
		m_audioPlayer = new quickevent::gui::audio::Player(this);
	return m_audioPlayer;
}

void CardReaderWidget::operatorAudioWakeUp()
{
	audioPlayer()->playAlert(quickevent::gui::audio::Player::AlertKind::OperatorWakeUp);
}

void CardReaderWidget::operatorAudioNotify()
{
	audioPlayer()->playAlert(quickevent::gui::audio::Player::AlertKind::OperatorNotify);
}

int CardReaderWidget::currentPunchMode()
{
	return m_cbxPunchMode->currentData().toInt();
}

static int msecToSISec(int msec)
{
	//static constexpr int secs_to_noon = 12 * 60 * 60;
	return (msec / 1000);// % secs_to_noon;
}

static int obStringTosec(const QString &time_str)
{
	bool ok;
	int min = time_str.section('.', 0, 0).toInt(&ok);
	int sec = 0;
	QF_ASSERT_EX(ok == true, QString("Cannot convert time string '%1'!").arg(time_str));
	QString sec_str = time_str.section('.', 1).trimmed();
	if(!sec_str.isEmpty()) {
		if(sec_str.length() == 1)
			sec_str = sec_str + '0';
		sec = sec_str.toInt(&ok);
		QF_ASSERT_EX(ok == true, QString("Cannot convert time string '%1'!").arg(time_str));
	}
	return (60 * min + sec) * 1000;
}

static QList<int> codesForClassName(const QString &class_name, int stage_id)
{
	QList<int> ret;
	int course_id = 0;
	{
		qfs::QueryBuilder qb;
		qb.select2("classdefs", "courseId")
				.from("classes")
				.joinRestricted("classes.id", "classdefs.classId", "classdefs.stageId=" QF_IARG(stage_id))
				.where("classes.name=" QF_SARG(class_name));
		qfs::Query q;
		q.exec(qb.toString(), qf::core::Exception::Throw);
		if(q.next())
			course_id = q.value(0).toInt();
	}
	QF_ASSERT_EX(course_id > 0, QString("Cannot find course for class %1 and stage %2").arg(class_name).arg(stage_id));
	{
		qfs::QueryBuilder qb;
		qb.select2("coursecodes", "position")
				.select2("codes", "code")
				.from("coursecodes")
				.join("coursecodes.codeId", "codes.id")
				.where("coursecodes.courseId=" QF_IARG(course_id))
				.orderBy("coursecodes.position");
		qfs::Query q;
		q.exec(qb.toString(), qf::core::Exception::Throw);
		while (q.next()) {
			int code = q.value("code").toInt();
			QF_ASSERT_EX(code > 0, "Code must be > 0");
			ret << code;
		}
	}
	QF_ASSERT_EX(ret.count() > 0, QString("Cannot load codes for class %1 and stage %2").arg(class_name).arg(stage_id));
	return ret;
}

void CardReaderWidget::importCards_lapsOnlyCsv()
{
	// CSV record must have format:
	// 7203463,"2,28","3,34","2,42","3,29","3,12","1,38","1,13","3,18","1,17","0,15"
	// CSV rows can be commented by #
	qfLogFuncFrame();
	qf::qmlwidgets::dialogs::MessageBox::showInfo(this, tr("<p>CSV record must have format:</p>"
														   "<p>7203463,\"2,28\",\"3,34\",\"2,42\",\"3,29\",\"3,12\",\"1,38\",\"1,13\",\"3,18\",\"1,17\",\"0,15\"</p>"
														   "<p>Any row can be commented by leading #</p>"
														   "<p>Decimal point is also supported, the quotes can be omited than.</p>"));
	QString fn = qf::qmlwidgets::dialogs::FileDialog::getOpenFileName(this, tr("Import CSV"));
	if(fn.isEmpty())
		return;
	QFile f(fn);
	if(!f.open(QFile::ReadOnly)) {
		qf::qmlwidgets::dialogs::MessageBox::showError(this, tr("Cannot open file '%1' for reading.").arg(f.fileName()));
		return;
	}
	QTextStream ts(&f);
	qf::core::utils::CSVReader reader(&ts);
	//reader.setSeparator(';');
	reader.setLineComment('#');
	try {
		qf::core::sql::Transaction transaction;
		while (!ts.atEnd()) {
			QStringList sl = reader.readCSVLineSplitted();
			// remove empty strings in the end of line
			int csv_ix = sl.indexOf(QString());
			if(csv_ix >= 0)
				sl = sl.mid(0, csv_ix);
			csv_ix = 0;
			int si_id = sl.value(csv_ix++).toInt();
			QF_ASSERT_EX(si_id > 0, "Bad SI!");
			int stage_id = getPlugin<EventPlugin>()->currentStageId();
			QF_ASSERT_EX(stage_id > 0, tr("Bad stage!"));
			int start00 = getPlugin<EventPlugin>()->stageStartMsec(stage_id);
			int run_id = 0;
			int start_time = 0;
			QString class_name;
			{
				qfs::QueryBuilder qb;
				qb.select2("runs", "id, startTimeMs")
						.select2("classes", "name")
						.from("runs")
						.innerJoin("runs.competitorId", "competitors.id")
						.innerJoin("competitors.classId", "classes.id")
						.where("runs.stageId=" QF_IARG(stage_id))
						.where("runs.siId=" QF_IARG(si_id)) ;
				qfs::Query q;
				q.exec(qb.toString(), qf::core::Exception::Throw);
				if(q.next()) {
					run_id = q.value("runs.id").toInt();
					start_time = start00 + q.value("runs.startTimeMs").toInt();
					class_name = q.value("classes.name").toString();
				}
			}
			QF_ASSERT_EX(run_id > 0, tr("Cannot find runs record for SI %1!").arg(si_id));
			QF_ASSERT_EX(!class_name.isEmpty(), tr("Cannot find class for SI %1!").arg(si_id));
			quickevent::core::si::ReadCard read_card;
			read_card.setCardNumber(si_id);
			read_card.setRunId(run_id);
			read_card.setStartTime(msecToSISec(start_time));
			read_card.setCheckTime(msecToSISec(start_time - (1000 * 90)));
			QVariantList punches;
			int stp_time = start_time;
			QList<int> codes = codesForClassName(class_name, stage_id);
			codes << quickevent::core::CodeDef::FINISH_PUNCH_CODE;
			if(csv_ix + codes.count() != sl.count()) {
				//qfWarning() << codes;
				qfWarning() << sl;
				QF_EXCEPTION(tr("SI: %1 class %2 - Number of punches (%3) and number of codes including finish (%4) should be the same! Remove or comment invalid line by #.")
							 .arg(si_id).arg(class_name).arg(sl.count() - csv_ix).arg(codes.count()));
			}
			for (int i = 0; i < codes.count(); ++i) {
				QString lap_str = sl.value(csv_ix++);
				lap_str.replace(',', '.');
				int lap_time = obStringTosec(lap_str);
				stp_time += lap_time;
				quickevent::core::si::ReadPunch punch;
				punch.setCode(codes[i]);
				punch.setTime(msecToSISec(stp_time));
				punches << punch;
			}
			read_card.setFinishTime(quickevent::core::si::ReadPunch(punches.takeLast().toMap()).time());
			read_card.setPunches(punches);
			qfDebug() << read_card.toString();
			processReadCard(read_card);
		}
		transaction.commit();
	}
	catch (const qf::core::Exception &e) {
		qf::qmlwidgets::dialogs::MessageBox::showException(this, e);
	}
}

void CardReaderWidget::importCards_SIReaderBackupMemoryCsv()
{
	qfLogFuncFrame();
	// No
	// Read on
	// SIID
	// Start no
	// Clear CN
	// Clear DOW
	// Clear time
	// Check CN
	// Check DOW
	// Check time
	// Start CN
	// Start DOW
	// Start time
	// Finish CN
	// Finish DOW
	// Finish time
	// Class
	// First name
	// Last name
	// Club
	// Country
	// Email
	// Date of birth
	// Sex
	// Phone
	// Street
	// ZIP
	// City
	// Hardware version
	// Software version
	// Battery date
	// Battery voltage
	// Clear count
	// Character set
	// SEL_FEEDBACK
	// No. of records
	// Record 1 CN
	// Record 1 DOW
	// Record 1 time
	// Record 2 CN
	// Record 2 DOW
	// Record 2 time
	// Record 3 CN
	// Record 3 DOW
	// Record 3 time
	// Record 4 CN
	// Record 4 DOW
	// Record 4 time
	// ...
	// Record 70 CN
	// Record 70 DOW
	// Record 70 time
	// Record 71 CN
	enum {
		col_No = 0,
		col_Read_on,
		col_SIID,
		col_Start_no,
		col_Clear_CN,
		col_Clear_DOW,
		col_Clear_time,
		col_Check_CN,
		col_Check_DOW,
		col_Check_time,
		col_Start_CN,
		col_Start_DOW,
		col_Start_time,
		col_Finish_CN,
		col_Finish_DOW,
		col_Finish_time,
		col_Class,
		col_First_name,
		col_Last_name,
		col_Club,
		col_Country,
		col_Email,
		col_Date_of_birth,
		col_Sex,
		col_Phone,
		col_Street,
		col_ZIP,
		col_City,
		col_Hardware_version,
		col_Software_version,
		col_Battery_date,
		col_Battery_voltage,
		col_Clear_count,
		col_Character_set,
		col_SEL_FEEDBACK,
		col_No_of_records,
		col_Record_1_CN,
		col_Record_1_DOW,
		col_Record_1_time,
	};
	//qf::qmlwidgets::dialogs::MessageBox::showInfo(this, tr("<p>CSV record must have format:</p>"
	//													   "<p>7203463,\"2,28\",\"3,34\",\"2,42\",\"3,29\",\"3,12\",\"1,38\",\"1,13\",\"3,18\",\"1,17\",\"0,15\"</p>"
	//													   "<p>Any row can be commented by leading #</p>"
	//													   "<p>Decimal point is also supported, the quotes can be omited than.</p>"));
	QString fn = qf::qmlwidgets::dialogs::FileDialog::getOpenFileName(this, tr("Import TXT"));
	if(fn.isEmpty())
		return;
	QFile f(fn);
	if(!f.open(QFile::ReadOnly)) {
		qf::qmlwidgets::dialogs::MessageBox::showError(this, tr("Cannot open file '%1' for reading.").arg(f.fileName()));
		return;
	}
	int stage_id = getPlugin<EventPlugin>()->currentStageId();
	QF_ASSERT_EX(stage_id > 0, tr("Bad stage!"));
	int start00 = getPlugin<EventPlugin>()->stageStartMsec(stage_id);

	QTextStream ts(&f);
	qf::core::utils::CSVReader reader(&ts);
	reader.setSeparator(';');
	reader.setLineComment('#');
	try {
		qf::core::sql::Transaction transaction;
		int line_no = 0;
		while (!ts.atEnd()) {
			QStringList sl = reader.readCSVLineSplitted();
			if(line_no++ == 0)
				continue; /// skip hading
			int si_id = sl.value(col_SIID).toInt();
			if(si_id == 0) {
				qfError() << "Ignoring bad SI:" << sl.value(col_SIID);
				continue;
			}
			int run_id = 0;
			int start_time = 0;
			QString class_name;
			{
				qfs::QueryBuilder qb;
				qb.select2("runs", "id, startTimeMs")
						.select2("classes", "name")
						.from("runs")
						.innerJoin("runs.competitorId", "competitors.id")
						.innerJoin("competitors.classId", "classes.id")
						.where("runs.stageId=" QF_IARG(stage_id))
						.where("runs.siId=" QF_IARG(si_id)) ;
				qfs::Query q;
				q.exec(qb.toString(), qf::core::Exception::Throw);
				if(q.next()) {
					run_id = q.value("runs.id").toInt();
					start_time = start00 + q.value("runs.startTimeMs").toInt();
					//class_name = q.value("classes.name").toString();
				}
			}
			if(run_id == 0) {
				qfError() << "Cannot find runs record for SI:" << si_id;
				continue;
			}
			quickevent::core::si::ReadCard read_card;
			read_card.setCardNumber(si_id);
			read_card.setRunId(run_id);
			auto time_to_msec = [](const QString &s, int default_msec) {
				QTime tm = QTime::fromString(s, Qt::ISODate);
				return tm.isValid()? tm.msecsSinceStartOfDay(): default_msec;
			};
			read_card.setStartTime(msecToSISec(time_to_msec(sl.value(col_Start_time), start_time)));
			read_card.setCheckTime(msecToSISec(time_to_msec(sl.value(col_Check_time), start_time - (1000 * 90))));
			read_card.setFinishTime(msecToSISec(time_to_msec(sl.value(col_Finish_time), 0)));
			QVariantList punches;
			int rec_cnt = sl.value(col_No_of_records).toInt();
			for (int i = 0; i < rec_cnt; ++i) {
				int code = sl.value(col_Record_1_CN + 3*i).toInt();
				int tm = msecToSISec(time_to_msec(sl.value(col_Record_1_time + 3*i), 0));
				quickevent::core::si::ReadPunch punch;
				punch.setCode(code);
				punch.setTime(tm);
				punches << punch;
			}
			read_card.setFinishTime(quickevent::core::si::ReadPunch(punches.takeLast().toMap()).time());
			read_card.setPunches(punches);
			qfDebug() << read_card.toString();
			processReadCard(read_card);
		}
		transaction.commit();
	}
	catch (const qf::core::Exception &e) {
		qf::qmlwidgets::dialogs::MessageBox::showException(this, e);
	}
}

void CardReaderWidget::readStationBackupMemory()
{
	siut::SiTaskReadStationBackupMemory *si_task = new siut::SiTaskReadStationBackupMemory();
	connect(si_task, &siut::SiTaskStationConfig::progress, this, [this, si_task](int phase, int count) {
		QProgressDialog *progress_dlg = this->findChild<QProgressDialog*>(QString(), Qt::FindDirectChildrenOnly);
		if(progress_dlg == nullptr) {
			progress_dlg = new QProgressDialog(tr("Downloading station backup ..."), "Abort", 0, count, this);
			connect(progress_dlg, &QProgressDialog::canceled, si_task, [si_task]() {
				si_task->abortWithMessage(tr("Cancelled by user"));
			});
			connect(si_task, &siut::SiTask::finished, progress_dlg, [progress_dlg]() {
				progress_dlg->setParent(nullptr);
				progress_dlg->deleteLater();
			});
		}
		progress_dlg->setValue(phase);
	});
	connect(si_task, &siut::SiTaskStationConfig::finished, this, [this](bool ok, QVariant result) {
		if(ok) {
			//qf::qmlwidgets::dialogs::MessageBox::showInfo(this, "memory read");
			QVariantMap m = result.toMap();
			int station_number = m.value("stationNumber").toInt();
			QVariantList punches = m.value("punches").toList();

			QVariantList html_rows;
			for (int i = 0; i < punches.size(); i++) {
				QVariantList punch = punches[i].toList();
				QVariantList tr{QStringLiteral("tr")};
				tr.insert(tr.length(), QVariantList{"td", QString::number(i+1)});

				int si = punch.value(0).toInt();
				tr.insert(tr.length(), QVariantList{"td", QString::number(si)});
				QDateTime punch_time = punch.value(1).toDateTime();
				tr.insert(tr.length(), QVariantList{"td", punch_time.toString(Qt::ISODateWithMs)});
				bool card_error = punch.value(2).toBool();
				tr.insert(tr.length(), QVariantList{"td", card_error? "Y": ""});
				html_rows.insert(html_rows.length(), tr);
			}
			const QStringList html_fields{tr("No."), tr("SI"), tr("DateTime"), tr("Card error")};
			QVariantList html_body = QVariantList() << QStringLiteral("body");
			html_body.insert(html_body.length(), QVariantList() << QStringLiteral("body"));
			html_body.insert(html_body.length(), qf::core::utils::HtmlUtils::createHtmlTable(tr("Station %1 backup memory").arg(station_number), html_fields, html_rows));
			qf::core::utils::HtmlUtils::FromHtmlListOptions opts;
			opts.setDocumentTitle(tr("Station backup memory"));
			QString html = qf::core::utils::HtmlUtils::fromHtmlList(html_body, opts);

			qf::qmlwidgets::dialogs::Dialog dlg(QDialogButtonBox::Save | QDialogButtonBox::Cancel, this);
			/*
			qf::qmlwidgets::DialogButtonBox *bbx = dlg.buttonBox();
			QPushButton *bt_set_off_race = new QPushButton(tr("Set off-race"));
			bt_set_off_race->setToolTip(tr("All runners without CHECK will be marked as off-race."));
			bool set_off_race = false;
			connect(bt_set_off_race, &QPushButton::clicked, [&set_off_race]() {
				set_off_race = true;
			});
			bbx->addButton(bt_set_off_race, QDialogButtonBox::AcceptRole);
			*/
			auto *w = new qf::qmlwidgets::HtmlViewWidget();
			dlg.setCentralWidget(w);
			w->setHtmlText(html);
			if(dlg.exec()) {
				// save checks
				try {
					qfs::Transaction transaction;
					int stage_id = getPlugin<EventPlugin>()->currentStageId();
					qfs::Query q1;
					q1.prepare("INSERT INTO stationsbackup (stageId, stationNumber, siId, punchDateTime, cardErr) VALUES"
							  " (:stageId, :stationNumber, :siId, :punchDateTime, :cardErr)"
							  , qfc::Exception::Throw);
					QDateTime stage_start_dt = getPlugin<EventPlugin>()->stageStartDateTime(stage_id);
					qfs::Query q2;
					q2.prepare("UPDATE runs SET checkTimeMs=:checkTimeMs"
							  " WHERE siId=:siId AND checkTimeMs IS NULL AND stageId=" QF_IARG(stage_id)
							  , qfc::Exception::Throw);
					for (int i = 0; i < punches.size(); i++) {
						QVariantList punch = punches[i].toList();
						q1.bindValue(QStringLiteral(":stageId"), stage_id);
						q1.bindValue(QStringLiteral(":stationNumber"), station_number);
						int si = punch.value(0).toInt();
						q1.bindValue(QStringLiteral(":siId"), si);
						QDateTime punch_time = punch.value(1).toDateTime();
						int check_time_msec = (int)stage_start_dt.msecsTo(punch_time);
						q1.bindValue(QStringLiteral(":punchDateTime"), punch_time);
						bool card_error = punch.value(2).toBool();

						q1.bindValue(QStringLiteral(":cardErr"), card_error);
						q1.exec(!qfc::Exception::Throw); // ignore unique key error

						q2.bindValue(QStringLiteral(":siId"), si);
						q2.bindValue(QStringLiteral(":checkTimeMs"), check_time_msec);
						q2.exec(qfc::Exception::Throw);
					}
					transaction.commit();
				}
				catch (qfc::Exception &ex) {
					qfd::MessageBox::showException(this, ex);
				}
			}
		}
		else {
			qfd::MessageBox::showError(this, result.toString());
		}
	}, Qt::QueuedConnection);
	siDriver()->setSiTask(si_task);
}

#include "cardreaderwidget.moc"

