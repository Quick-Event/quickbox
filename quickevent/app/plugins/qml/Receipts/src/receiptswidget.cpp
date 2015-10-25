#include "receiptswidget.h"
#include "ui_receiptswidget.h"
#include "receiptspartwidget.h"
#include "receiptsprinteroptionsdialog.h"
#include "receiptsprinter.h"
#include "receiptsprinteroptions.h"
#include "Receipts/receiptsplugin.h"

#include <Event/eventplugin.h>
#include <CardReader/cardreaderplugin.h>

#include <quickevent/og/timems.h>
#include <quickevent/og/sqltablemodel.h>
#include <quickevent/og/itemdelegate.h>
#include <quickevent/og/siid.h>

#include <qf/qmlwidgets/action.h>
#include <qf/qmlwidgets/framework/application.h>
#include <qf/qmlwidgets/framework/partwidget.h>
#include <qf/qmlwidgets/framework/mainwindow.h>
#include <qf/qmlwidgets/menubar.h>
#include <qf/qmlwidgets/toolbar.h>

#include <qf/core/log.h>
#include <qf/core/exception.h>
#include <qf/core/sql/connection.h>
#include <qf/core/sql/query.h>
#include <qf/core/sql/dbenum.h>
#include <qf/core/model/sqltablemodel.h>

#include <QSettings>
#include <QFile>
#include <QTextStream>
#include <QComboBox>
#include <QLabel>
#include <QMetaObject>
#include <QJSValue>
#include <QPrinterInfo>
#include <QTimer>

namespace qfm = qf::core::model;
namespace qfs = qf::core::sql;
namespace qff = qf::qmlwidgets::framework;
namespace qfw = qf::qmlwidgets;

const char *ReceiptsWidget::SETTINGS_PREFIX = "plugins/Receipts";

ReceiptsWidget::ReceiptsWidget(QWidget *parent) :
	Super(parent),
	ui(new Ui::ReceiptsWidget)
{
	ui->setupUi(this);
	//ui->cbxDirectPrinters->setCurrentIndex(2);
	createActions();

	{
		ui->tblPrintJobsTB->setTableView(ui->tblCards);

		ui->tblCards->setInsertRowEnabled(false);
		ui->tblCards->setRemoveRowEnabled(false);
		ui->tblCards->setCloneRowEnabled(false);
		ui->tblCards->setPersistentSettingsId(ui->tblCards->objectName());
		//ui->tblPrintJobs->setRowEditorMode(qfw::TableView::EditRowsMixed);
		ui->tblCards->setInlineEditSaveStrategy(qfw::TableView::OnEditedValueCommit);
		ui->tblCards->setItemDelegate(new quickevent::og::ItemDelegate(ui->tblCards));
		auto m = new quickevent::og::SqlTableModel(this);

		m->addColumn("cards.id", "ID").setReadOnly(true);
		m->addColumn("cards.siId", tr("SI")).setReadOnly(true).setCastType(qMetaTypeId<quickevent::og::SiId>());
		m->addColumn("classes.name", tr("Class"));
		m->addColumn("competitorName", tr("Name"));
		m->addColumn("competitors.registration", tr("Reg"));
		m->addColumn("runs.startTimeMs", tr("Start")).setCastType(qMetaTypeId<quickevent::og::TimeMs>());
		m->addColumn("runs.timeMs", tr("Time")).setCastType(qMetaTypeId<quickevent::og::TimeMs>());
		/*
		qfm::SqlTableModel::ColumnDefinition::DbEnumCastProperties status_props;
		status_props.setGroupName("runs.status");
		m->addColumn("runs.status", tr("Status"))
				.setCastType(qMetaTypeId<qf::core::sql::DbEnum>(), status_props);
		*/
		m->addColumn("cards.printerConnectionId", tr("printer"));

		ui->tblCards->setTableModel(m);
		m_cardsModel = m;
	}

	ui->tblCards->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(ui->tblCards, &qfw::TableView::customContextMenuRequested, this, &ReceiptsWidget::onCustomContextMenuRequest);

	QTimer::singleShot(0, this, &ReceiptsWidget::lazyInit);
}

ReceiptsWidget::~ReceiptsWidget()
{
	delete ui;
}

void ReceiptsWidget::lazyInit()
{
	updateReceiptsPrinterLabel();
}

void ReceiptsWidget::settleDownInPartWidget(ReceiptsPartWidget *part_widget)
{
	connect(part_widget, SIGNAL(resetPartRequest()), this, SLOT(reset()));
	connect(part_widget, SIGNAL(reloadPartRequest()), this, SLOT(reset()));

	connect(eventPlugin(), SIGNAL(dbEventNotify(QString,QVariant)), this, SLOT(onDbEventNotify(QString,QVariant)), Qt::QueuedConnection);
}

void ReceiptsWidget::reset()
{
	if(eventPlugin()->eventName().isEmpty()) {
		m_cardsModel->clearRows();
		return;
	}
	reload();
}

void ReceiptsWidget::reload()
{
	int current_stage = currentStageId();
	qfs::QueryBuilder qb;
	qb.select2("cards", "id, siId, printerConnectionId")
			.select2("runs", "startTimeMs, timeMs")
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

Receipts::ReceiptsPlugin *ReceiptsWidget::receiptsPlugin()
{
	qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
	auto plugin = qobject_cast<Receipts::ReceiptsPlugin *>(fwk->plugin("Receipts"));
	QF_ASSERT(plugin != nullptr, "Bad plugin", return nullptr);
	return plugin;
}

Event::EventPlugin *ReceiptsWidget::eventPlugin()
{
	qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
	auto plugin = qobject_cast<Event::EventPlugin *>(fwk->plugin("Event"));
	QF_ASSERT(plugin != nullptr, "Bad plugin", return nullptr);
	return plugin;
}

void ReceiptsWidget::onDbEventNotify(const QString &domain, const QVariant &payload)
{
	Q_UNUSED(payload)
	if(domain == QLatin1String(CardReader::CardReaderPlugin::DBEVENTDOMAIN_CARDREADER_CARDREAD)) {
		onCardRead();
	}
}

void ReceiptsWidget::createActions()
{
	//QStyle *sty = style();
	/*
	{
		QIcon ico(":/quickevent/Receipts/images/comm");
		qf::qmlwidgets::Action *a = new qf::qmlwidgets::Action(ico, tr("Open COM"), this);
		a->setCheckable(true);
		connect(a, SIGNAL(triggered(bool)), this, SLOT(onCommOpen(bool)));
		m_actCommOpen = a;
	}
	*/
}

int ReceiptsWidget::currentStageId()
{
	auto event_plugin = eventPlugin();
	QF_ASSERT(event_plugin != nullptr, "Bad plugin", return 0);
	int ret = event_plugin->currentStageId();
	return ret;
}

void ReceiptsWidget::onCardRead()
{
	if(ui->chkAutoPrint->isChecked()) {
		printNewCards();
	}
	loadNewCards();
}

void ReceiptsWidget::printNewCards()
{
	auto conn  = qf::core::sql::Connection::forName();
	int connection_id = 1;
	if(!conn.driverName().endsWith("SQLITE"))
		connection_id = conn.connectionId();
	QF_ASSERT(connection_id > 0, "Cannot get SQL connection id", return);
	int current_stage = currentStageId();
	QString qs = "UPDATE cards SET printerConnectionId=" QF_IARG(connection_id)
			" WHERE printerConnectionId IS NULL"
			" AND cards.stageId=" QF_IARG(current_stage);
	qf::core::sql::Query q(conn);
	q.exec(qs, qf::core::Exception::Throw);
	int num_rows = q.numRowsAffected();
	if(num_rows > 0) {
		qs = "SELECT id FROM cards WHERE printerConnectionId=" QF_IARG(connection_id)
				" ORDER BY id DESC"
				" LIMIT " QF_IARG(num_rows);
		q.exec(qs, qf::core::Exception::Throw);
		while(q.next()) {
			int card_id = q.value(0).toInt();
			if(!printReceipt(card_id)) {
				break;
			}
		}
	}
}

void ReceiptsWidget::loadNewCards()
{
	m_cardsModel->reloadInserts(QStringLiteral("cards.id"));
}

void ReceiptsWidget::on_btPrintNew_clicked()
{
	printNewCards();
	loadNewCards();
}

void ReceiptsWidget::onCustomContextMenuRequest(const QPoint &pos)
{
	qfLogFuncFrame();
	QAction a_print_card(tr("Print selected cards"), nullptr);
	QList<QAction*> lst;
	lst << &a_print_card;
	QAction *a = QMenu::exec(lst, ui->tblCards->viewport()->mapToGlobal(pos));
	if(a == &a_print_card) {
		printSelectedCards();
	}
}

void ReceiptsWidget::printSelectedCards()
{
	for(int i : ui->tblCards->selectedRowsIndexes()) {
		int card_id = ui->tblCards->tableRow(i).value("cards.id").toInt();
		QF_ASSERT(card_id > 0, "Bad card ID", return);
		printReceipt(card_id);
	}
}

bool ReceiptsWidget::printReceipt(int card_id)
{
	return receiptsPlugin()->printReceipt(card_id);
}

void ReceiptsWidget::updateReceiptsPrinterLabel()
{
	const auto &opts = receiptsPlugin()->receiptsPrinter()->printerOptions();
	ui->btPrinterOptions->setText(opts.printerCaption());
	if(opts.printerType() == (int)ReceiptsPrinterOptions::PrinterType::GraphicPrinter)
		ui->btPrinterOptions->setIcon(QIcon(":/quickevent/Receipts/images/graphic-printer.svg"));
	else
		ui->btPrinterOptions->setIcon(QIcon(":/quickevent/Receipts/images/character-printer.svg"));
}

void ReceiptsWidget::on_btPrinterOptions_clicked()
{
	ReceiptsPrinterOptionsDialog dlg(this);
	dlg.setPrinterOptions(receiptsPlugin()->receiptsPrinter()->printerOptions());
	if(dlg.exec()) {
		receiptsPlugin()->setReceiptsPrinterOptions(dlg.printerOptions());
		updateReceiptsPrinterLabel();
	}
}
