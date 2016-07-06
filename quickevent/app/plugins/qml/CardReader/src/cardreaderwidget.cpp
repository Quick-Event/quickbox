#include "cardreaderwidget.h"
#include "ui_cardreaderwidget.h"
#include "dlgsettings.h"
#include "cardreaderpartwidget.h"
#include "CardReader/cardreaderplugin.h"
#include "CardReader/cardchecker.h"
#include "CardReader/readcard.h"
#include "CardReader/checkedcard.h"

#include <Event/eventplugin.h>
#include <Runs/findrunnerwidget.h>

#include <quickevent/og/timems.h>
#include <quickevent/og/sqltablemodel.h>
#include <quickevent/og/itemdelegate.h>
#include <quickevent/si/punchrecord.h>
#include <quickevent/si/siid.h>
#include <quickevent/audio/player.h>

#include <siut/sidevicedriver.h>
#include <siut/simessage.h>

#include <qf/qmlwidgets/action.h>
#include <qf/qmlwidgets/framework/application.h>
#include <qf/qmlwidgets/framework/partwidget.h>
#include <qf/qmlwidgets/framework/mainwindow.h>
#include <qf/qmlwidgets/combobox.h>
#include <qf/qmlwidgets/menubar.h>
#include <qf/qmlwidgets/toolbar.h>
#include <qf/qmlwidgets/dialogs/dialog.h>
#include <qf/qmlwidgets/dialogs/messagebox.h>
#include <qf/qmlwidgets/dialogs/filedialog.h>

#include <qf/core/log.h>
#include <qf/core/assert.h>
#include <qf/core/exception.h>
#include <qf/core/sql/query.h>
#include <qf/core/sql/dbenum.h>
#include <qf/core/sql/transaction.h>
#include <qf/core/model/sqltablemodel.h>
#include <qf/core/utils/settings.h>
#include <qf/core/utils/csvreader.h>

#include <QSettings>
#include <QFile>
#include <QTextStream>
#include <QComboBox>
#include <QLabel>
#include <QCheckBox>

namespace qfm = qf::core::model;
namespace qfs = qf::core::sql;
namespace qff = qf::qmlwidgets::framework;
namespace qfw = qf::qmlwidgets;
namespace qfd = qf::qmlwidgets::dialogs;

static Event::EventPlugin* eventPlugin()
{
	qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
	auto *plugin = qobject_cast<Event::EventPlugin*>(fwk->plugin("Event"));
	QF_ASSERT_EX(plugin != nullptr, "Bad Event plugin!");
	return plugin;
}

namespace {
class Model : public quickevent::og::SqlTableModel
{
	Q_OBJECT
private:
	typedef quickevent::og::SqlTableModel Super;
public:
	explicit Model(QObject *parent) : Super(parent) {}

	QVariant data(const QModelIndex &index, int role) const Q_DECL_OVERRIDE;
};

QVariant Model::data(const QModelIndex &index, int role) const
{
	if(role == Qt::BackgroundRole) {
		static auto C_RUNID = QStringLiteral("cards.runId");
		int run_id = tableRow(index.row()).value(C_RUNID).toInt();
		if(run_id == 0) {
			static auto c = QColor(Qt::red).lighter(150);
			return c;
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
		ui->tblCards->setInlineEditSaveStrategy(qfw::TableView::OnEditedValueCommit);
		ui->tblCards->setItemDelegate(new quickevent::og::ItemDelegate(ui->tblCards));
		auto m = new Model(this);
		m->addColumn("cards.id", "ID").setReadOnly(true);
		m->addColumn("cards.siId", tr("SI")).setReadOnly(true).setCastType(qMetaTypeId<quickevent::si::SiId>());
		m->addColumn("classes.name", tr("Class"));
		m->addColumn("competitorName", tr("Name"));
		m->addColumn("competitors.registration", tr("Reg"));
		m->addColumn("runs.startTimeMs", tr("Start")).setCastType(qMetaTypeId<quickevent::og::TimeMs>()).setReadOnly(true);
		m->addColumn("runs.timeMs", tr("Time")).setCastType(qMetaTypeId<quickevent::og::TimeMs>()).setReadOnly(true);
		m->addColumn("runs.finishTimeMs", tr("Finish")).setCastType(qMetaTypeId<quickevent::og::TimeMs>()).setReadOnly(true);
		m->addColumn("runs.misPunch", tr("Error")).setToolTip(tr("Card mispunch")).setReadOnly(true);
		m->addColumn("runs.disqualified", tr("DISQ")).setToolTip(tr("Disqualified"));
		m->addColumn("runs.cardLent", tr("L")).setToolTip(tr("Card lent")).setReadOnly(true);
		m->addColumn("runs.cardReturned", tr("R")).setToolTip(tr("Card returned"));
		/*
		qfm::SqlTableModel::ColumnDefinition::DbEnumCastProperties status_props;
		status_props.setGroupName("runs.status");
		m->addColumn("runs.status", tr("Status"))
				.setCastType(qMetaTypeId<qf::core::sql::DbEnum>(), status_props);
		*/
		ui->tblCards->setTableModel(m);
		m_cardsModel = m;
	}
	ui->tblCards->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(ui->tblCards, &qfw::TableView::customContextMenuRequested, this, &CardReaderWidget::onCustomContextMenuRequest);
}

void CardReaderWidget::onCustomContextMenuRequest(const QPoint & pos)
{
	qfLogFuncFrame();
	QAction a_show_receipt(tr("Show receipt"), nullptr);
	QAction a_show_card(tr("Show card"), nullptr);
	QAction a_print_receipt(tr("Print receipt"), nullptr);
	QAction a_assign_runner(tr("Assign card to runner"), nullptr);
	QList<QAction*> lst;
	lst << &a_show_receipt << &a_print_receipt << &a_show_card << &a_assign_runner;
	QAction *a = QMenu::exec(lst, ui->tblCards->viewport()->mapToGlobal(pos));
	if(a == &a_show_receipt) {
		showSelectedReceipt();
	}
	if(a == &a_show_card) {
		showSelectedCard();
	}
	else if(a == &a_print_receipt) {
		qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
		auto *plugin = fwk->plugin("Receipts");
		if(!plugin) {
			qfError() << "Cannot find Receipts plugin!";
			return;
		}
		int card_id = ui->tblCards->tableRow().value("cards.id").toInt();
		QMetaObject::invokeMethod(plugin, "printReceipt", Q_ARG(int, card_id));
	}
	else if(a == &a_assign_runner) {
		assignRunnerToSelectedCard();
	}
}

CardReaderWidget::~CardReaderWidget()
{
	delete ui;
}

void CardReaderWidget::settleDownInPartWidget(CardReaderPartWidget *part_widget)
{
	connect(part_widget, SIGNAL(resetPartRequest()), this, SLOT(reset()));
	connect(part_widget, SIGNAL(reloadPartRequest()), this, SLOT(reset()));
	{
		qfw::Action *a = part_widget->menuBar()->actionForPath("station", true);
		a->setText("&Station");
		a->addActionInto(m_actCommOpen);
	}
	{
		qfw::Action *a_tools = part_widget->menuBar()->actionForPath("tools", true);
		a_tools->setText("&Tools");
		a_tools->addActionInto(m_actSettings);
		{
			auto *m_import_cards = a_tools->addMenuInto("importCards", tr("Import cards"));
			{
				qfw::Action *a = new qfw::Action("Laps only CSV");
				connect(a, &qf::qmlwidgets::Action::triggered, this, &CardReaderWidget::importCards_lapsOnlyCsv);
				m_import_cards->addActionInto(a);
			}
		}
		{
			qfw::Action *a = new qfw::Action("Test audio");
			connect(a, &qf::qmlwidgets::Action::triggered, this, &CardReaderWidget::operatorAudioNotify);
			a_tools->addActionInto(a);
		}
	}
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
	main_tb->addSeparator();
	{
		m_cbxAutoRefresh = new QCheckBox();
		m_cbxAutoRefresh->setText(tr("Auto refresh"));
		main_tb->addWidget(m_cbxAutoRefresh);
	}
	main_tb->addSeparator();
	{
		QLabel *lbl = new QLabel(" Reader mode ");
		main_tb->addWidget(lbl);
		m_cbxPunchMarking = new QComboBox();
		m_cbxPunchMarking->addItem(tr("Race"), quickevent::si::PunchRecord::MARKING_RACE);
		m_cbxPunchMarking->addItem(tr("Entries"), quickevent::si::PunchRecord::MARKING_ENTRIES);
		main_tb->addWidget(m_cbxPunchMarking);
	}
	connect(eventPlugin(), &Event::EventPlugin::dbEventNotify, this, &CardReaderWidget::onDbEventNotify, Qt::QueuedConnection);
}

void CardReaderWidget::reset()
{
	if(!eventPlugin()->isEventOpen()) {
		m_cardsModel->clearRows();
		return;
	}
	reload();
}

void CardReaderWidget::reload()
{
	int current_stage = thisPlugin()->currentStageId();
	qfs::QueryBuilder qb;
	qb.select2("cards", "id, siId, runId")
			.select2("runs", "id, startTimeMs, timeMs, finishTimeMs, misPunch, disqualified, cardLent, cardReturned")
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

void CardReaderWidget::onDbEventNotify(const QString &domain, const QVariant &payload)
{
	int card_id = payload.toInt();
	if(domain == QLatin1String(Event::EventPlugin::DBEVENT_CARD_READ)) {
		// TODO: only if widget is visible (plugin window active)
		if(m_cbxAutoRefresh->isChecked())
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
		//closeCardLog();
	}
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
		settings.beginGroup(CardReader::CardReaderPlugin::SETTINGS_PREFIX);
		settings.beginGroup("comm");
		settings.beginGroup("connection");
		QString device = settings.value("device", "").toString();
		int baud_rate = settings.value("baudRate", 38400).toInt();
		int data_bits = settings.value("dataBits", 8).toInt();
		int stop_bits = settings.value("stopBits", 1).toInt();
		QString parity = settings.value("parity", "none").toString();
		if(!siDriver()->openCommPort(device, baud_rate, data_bits, parity, stop_bits > 1)) {
			qf::qmlwidgets::dialogs::MessageBox::showError(this, tr("Error open device %1 - %2").arg(device).arg(siDriver()->commPortErrorString()));
		}
		//theApp()->scriptDriver()->callExtensionFunction("onCommConnect", QVariantList() << device);
	}
	else {
		siDriver()->closeCommPort();
	}
}

void CardReaderWidget::appendLog(qf::core::Log::Level level, const QString& msg)
{
	switch (level) {
	case qf::core::Log::Level::Debug: qfDebug() << msg; break;
	case qf::core::Log::Level::Info: qfInfo() << msg; break;
	case qf::core::Log::Level::Warning: qfWarning() << msg; break;
	default: qfError() << msg; break;
	}
}

void CardReaderWidget::processSIMessage(const SIMessageData& msg_data)
{
	qfLogFuncFrame();
	//appendLog(qf::core::Log::Level::Info, trUtf8("processSIMessage command: %1 , type: %2").arg(SIMessageData::commandName(msg_data.command())).arg(msg_data.type()));
	if(msg_data.type() == SIMessageData::MsgCardReadOut) {
		SIMessageCardReadOut card(msg_data);
		processSICard(card);
	}
	else if(msg_data.type() == SIMessageData::MsgCardEvent) {
		appendLog(qf::core::Log::Level::Debug, msg_data.dump());
		if(msg_data.command() == SIMessageData::CmdSICard5DetectedExt) {
			emit sendSICommand(SIMessageData::CmdGetSICard5Ext, QByteArray());
		}
		else if(msg_data.command() == SIMessageData::CmdSICard6DetectedExt) {
			emit sendSICommand(SIMessageData::CmdGetSICard6Ext, QByteArray("\x08", 1));
		}
		else if(msg_data.command() == SIMessageData::CmdSICard8AndHigherDetectedExt) {
			emit sendSICommand(SIMessageData::CmdGetSICard8Ext, QByteArray("\x08", 1));
		}
	}
	else if(msg_data.type() == SIMessageData::MsgPunch) {
		SIMessageTransmitPunch rec(msg_data);
		processSIPunch(rec);
	}
	else {
		appendLog(qf::core::Log::Level::Debug, msg_data.dump());
	}
}

void CardReaderWidget::processDriverInfo (qf::core::Log::Level level, const QString& msg )
{
	qfLogFuncFrame() << qf::core::Log::levelName(level) << msg;
	appendLog(level, trUtf8("DriverInfo: <%1> %2").arg(qf::core::Log::levelName((qf::core::Log::Level)level)).arg(msg));
}

void CardReaderWidget::processDriverRawData(const QByteArray& data)
{
	qf::core::utils::Settings settings;
	//qInfo() << settings.value(CardReader::CardReaderPlugin::SETTINGS_PREFIX + "/comm/debug/showRawComData") << "data:" << data;
	if(settings.value(CardReader::CardReaderPlugin::SETTINGS_PREFIX + "/comm/debug/showRawComData").toBool()) {
		QString msg = SIMessageData::dumpData(data);
		appendLog(qf::core::Log::Level::Info, trUtf8("DriverRawData: %1").arg(msg));
	}
}

void CardReaderWidget::processSICard(const SIMessageCardReadOut &card)
{
	appendLog(qf::core::Log::Level::Debug, card.dump());
	appendLog(qf::core::Log::Level::Info, trUtf8("card: %1").arg(card.cardNumber()));

	QString punch_marking = m_cbxPunchMarking->currentData().toString();
	if(punch_marking == quickevent::si::PunchRecord::MARKING_ENTRIES) {
		// send fake punch in the 'entries' mode to enable edit_competitor_by_punch function
		quickevent::si::PunchRecord punch;
		punch.setsiid(card.cardNumber());
		punch.setmarking(punch_marking);
		int punch_id = thisPlugin()->savePunchRecordToSql(punch);
		if(punch_id > 0) {
			punch["sqlId"] = punch_id;
			eventPlugin()->emitDbEvent(Event::EventPlugin::DBEVENT_PUNCH_RECEIVED, punch, true);
		}
		return;
	}

	int run_id = thisPlugin()->findRunId(card.cardNumber());
	if(run_id == 0) {
		operatorAudioWakeUp();
		appendLog(qf::core::Log::Level::Error, trUtf8("Cannot find run for SI: %1").arg(card.cardNumber()));
	}
	else {
		qf::core::sql::Query q;
		q.exec("SELECT cardLent, cardReturned FROM runs WHERE id=" QF_IARG(run_id) );
		if(q.next()) {
			if(q.value(0).toBool() && !q.value(1).toBool())
				operatorAudioNotify();
		}
	}
	CardReader::ReadCard read_card(card);
	read_card.setRunId(run_id);
	processReadCardSafe(read_card);
}

bool CardReaderWidget::processReadCardSafe(const CardReader::ReadCard &read_card)
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

void CardReaderWidget::processReadCard(const CardReader::ReadCard &read_card) throw(qf::core::Exception)
{
	int card_id = thisPlugin()->saveCardToSql(read_card);
	if(read_card.runId()) {
		thisPlugin()->saveCardAssignedRunnerIdSql(card_id, read_card.runId());
		CardReader::CheckedCard checked_card = thisPlugin()->checkCard(read_card);
		thisPlugin()->updateCheckedCardValuesSql(checked_card);
	}
	if(card_id > 0) {
		eventPlugin()->emitDbEvent(Event::EventPlugin::DBEVENT_CARD_READ, card_id, true);
	}
}

void CardReaderWidget::processSIPunch(const SIMessageTransmitPunch &rec)
{
	appendLog(qf::core::Log::Level::Info, trUtf8("punch: %1 %2").arg(rec.cardNumber()).arg(rec.punch().toString()));
	quickevent::si::PunchRecord punch(rec);
	QString punch_marking = m_cbxPunchMarking->currentData().toString();
	punch.setmarking(punch_marking);
	if(punch_marking == quickevent::si::PunchRecord::MARKING_RACE) {
		int run_id = thisPlugin()->findRunId(rec.cardNumber());
		if(run_id == 0)
			appendLog(qf::core::Log::Level::Error, trUtf8("Cannot find run for punch record SI: %1").arg(rec.cardNumber()));
		else
			punch.setrunid(run_id);
	}
	int punch_id = thisPlugin()->savePunchRecordToSql(punch);
	if(punch_id > 0) {
		punch["sqlId"] = punch_id;
		eventPlugin()->emitDbEvent(Event::EventPlugin::DBEVENT_PUNCH_RECEIVED, punch, true);
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

CardReader::CardReaderPlugin *CardReaderWidget::thisPlugin()
{
	qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
	auto cardreader_plugin = qobject_cast<CardReader::CardReaderPlugin *>(fwk->plugin("CardReader"));
	QF_ASSERT_EX(cardreader_plugin != nullptr, "Bad plugin");
	return cardreader_plugin;
}

qf::qmlwidgets::framework::Plugin *CardReaderWidget::receiptsPlugin()
{
	qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
	auto plugin = qobject_cast<qf::qmlwidgets::framework::Plugin *>(fwk->plugin("Receipts"));
	QF_ASSERT(plugin != nullptr, "Bad plugin", return nullptr);
	return plugin;
}

void CardReaderWidget::onCbxCardCheckersActivated(int ix)
{
	thisPlugin()->setCurrentCardCheckerIndex(ix);
}

void CardReaderWidget::showSelectedReceipt()
{
	qfLogFuncFrame();
	auto receipts_plugin = receiptsPlugin();
	if(!receipts_plugin)
		return;
	int card_id = ui->tblCards->selectedRow().value("cards.id").toInt();
	QMetaObject::invokeMethod(receipts_plugin, "previewReceipt", Q_ARG(int, card_id));
}

void CardReaderWidget::showSelectedCard()
{
	qfLogFuncFrame();
	auto receipts_plugin = receiptsPlugin();
	if(!receipts_plugin)
		return;
	int card_id = ui->tblCards->selectedRow().value("cards.id").toInt();
	QMetaObject::invokeMethod(receipts_plugin, "previewCard", Q_ARG(int, card_id));
}

void CardReaderWidget::assignRunnerToSelectedCard()
{
	qfLogFuncFrame();
	int card_id = ui->tblCards->tableRow().value("cards.id").toInt();
	QF_ASSERT(card_id > 0, "Bad card id!", return);
	auto *w = new Runs::FindRunnerWidget(eventPlugin()->currentStageId());
	w->setWindowTitle(tr("Find runner"));
	qfd::Dialog dlg(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
	dlg.setPersistentSettingsId("dlgAssignRunnerToSelectedCard");
	//dlg.setDefaultButton(QDialogButtonBox::Ok);
	dlg.setCentralWidget(w);
	//w->setFocusToWidget(Runs::FindRunnerWidget::FocusWidget::Name);
	w->focusLineEdit();
	connect(w, &Runs::FindRunnerWidget::runnerSelected, [this, card_id, &dlg](const QVariantMap &values) {
		dlg.accept();
		int run_id = values.value("runid").toInt();
		if(run_id) {
			CardReader::CheckedCard checked_card = thisPlugin()->checkCard(card_id, run_id);
			if(thisPlugin()->updateCheckedCardValuesSqlSafe(checked_card)) {
				if(thisPlugin()->saveCardAssignedRunnerIdSql(card_id, run_id))
					this->ui->tblCards->reloadRow();
			}
		}
	});
	dlg.exec();
}

quickevent::audio::Player *CardReaderWidget::audioPlayer()
{
	if(!m_audioPlayer)
		m_audioPlayer = new quickevent::audio::Player(this);
	return m_audioPlayer;
}

void CardReaderWidget::operatorAudioWakeUp()
{
	audioPlayer()->playAlert(quickevent::audio::Player::AlertKind::OperatorWakeUp);
}

void CardReaderWidget::operatorAudioNotify()
{
	audioPlayer()->playAlert(quickevent::audio::Player::AlertKind::OperatorNotify);
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
			int stage_id = eventPlugin()->currentStageId();
			QF_ASSERT_EX(stage_id > 0, tr("Bad stage!"));
			int start00 = eventPlugin()->stageStartMsec(stage_id);
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
			CardReader::ReadCard read_card;
			read_card.setCardNumber(si_id);
			read_card.setRunId(run_id);
			read_card.setStartTime(msecToSISec(start_time));
			read_card.setCheckTime(msecToSISec(start_time - (1000 * 90)));
			QVariantList punches;
			int stp_time = start_time;
			QList<int> codes = codesForClassName(class_name, stage_id);
			codes << CardReader::CardReaderPlugin::FINISH_PUNCH_CODE;
			if(csv_ix + codes.count() != sl.count()) {
				qfWarning() << codes;
				qfWarning() << sl;
				QF_EXCEPTION(tr("SI: %1 class %2 - Number of punches (%3) and number of codes including finish (%4) should be the same! Remove or comment invalid line by #.")
							 .arg(si_id).arg(class_name).arg(sl.count() - csv_ix).arg(codes.count()));
			}
			for (int i = 0; i < codes.count(); ++i) {
				QString lap_str = sl.value(csv_ix++);
				lap_str.replace(',', '.');
				int lap_time = obStringTosec(lap_str);
				stp_time += lap_time;
				CardReader::ReadPunch punch;
				punch.setCode(codes[i]);
				punch.setTime(msecToSISec(stp_time));
				punches << punch;
			}
			read_card.setFinishTime(CardReader::ReadPunch(punches.takeLast().toMap()).time());
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

#include "cardreaderwidget.moc"

