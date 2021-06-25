#include "receiptswidget.h"
#include "ui_receiptswidget.h"
#include "receiptsprinteroptionsdialog.h"
#include "receiptsprinter.h"
#include "receiptsprinteroptions.h"
#include "receiptsplugin.h"

#include <quickevent/gui/og/itemdelegate.h>

#include <quickevent/core/og/timems.h>
#include <quickevent/core/og/sqltablemodel.h>
#include <quickevent/core/si/siid.h>

#include <qf/qmlwidgets/action.h>
#include <qf/qmlwidgets/framework/application.h>
#include <qf/qmlwidgets/framework/partwidget.h>
#include <qf/qmlwidgets/framework/mainwindow.h>
#include <qf/qmlwidgets/menubar.h>
#include <qf/qmlwidgets/toolbar.h>

#include <qf/core/log.h>
#include <qf/core/assert.h>
#include <qf/core/exception.h>
#include <qf/core/sql/connection.h>
#include <qf/core/sql/query.h>
#include <qf/core/sql/dbenum.h>
#include <qf/core/model/sqltablemodel.h>
#include <plugins/Event/src/eventplugin.h>

#include <QSettings>
#include <QFile>
#include <QTextStream>
#include <QComboBox>
#include <QLabel>
#include <QMetaObject>
#include <QJSValue>
#include <QPrinterInfo>
#include <QTimer>
#include <QDirIterator>

namespace qfm = qf::core::model;
namespace qfs = qf::core::sql;
namespace qff = qf::qmlwidgets::framework;
namespace qfw = qf::qmlwidgets;
using qf::qmlwidgets::framework::getPlugin;
using Event::EventPlugin;
using Receipts::ReceiptsPlugin;

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
		ui->tblCards->setItemDelegate(new quickevent::gui::og::ItemDelegate(ui->tblCards));
		auto m = new quickevent::core::og::SqlTableModel(this);

		m->addColumn("cards.id", "ID").setReadOnly(true);
		m->addColumn("cards.siId", tr("SI")).setReadOnly(true).setCastType(qMetaTypeId<quickevent::core::si::SiId>());
		m->addColumn("classes.name", tr("Class"));
		m->addColumn("competitorName", tr("Name"));
		m->addColumn("competitors.registration", tr("Reg"));
		m->addColumn("runs.startTimeMs", tr("Start")).setCastType(qMetaTypeId<quickevent::core::og::TimeMs>());
		m->addColumn("runs.timeMs", tr("Time")).setCastType(qMetaTypeId<quickevent::core::og::TimeMs>());
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

	ui->lstNotFound->addItem(tr("Error info"), 0);
	ui->lstNotFound->addItem(tr("Receipt without name"), 1);
	ui->lstNotFound->setCurrentIndex(0);

	ui->tblCards->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(ui->tblCards, &qfw::TableView::customContextMenuRequested, this, &ReceiptsWidget::onCustomContextMenuRequest);

	connect(ui->lstReceipt, static_cast<void (QComboBox::*)(int)>(&QComboBox::activated), [this](int ix) {
		getPlugin<ReceiptsPlugin>()->setCurrentReceiptPath(ui->lstReceipt->itemData(ix).toString());
	});

	QTimer::singleShot(0, this, &ReceiptsWidget::lazyInit);
}

ReceiptsWidget::~ReceiptsWidget()
{
	delete ui;
}

void ReceiptsWidget::lazyInit()
{
	loadReceptList();
	updateReceiptsPrinterLabel();
}

void ReceiptsWidget::settleDownInPartWidget(quickevent::gui::PartWidget *part_widget)
{
	connect(part_widget, SIGNAL(resetPartRequest()), this, SLOT(reset()));
	connect(part_widget, SIGNAL(reloadPartRequest()), this, SLOT(reload()));

	connect(getPlugin<EventPlugin>(), &Event::EventPlugin::dbEventNotify, this, &ReceiptsWidget::onDbEventNotify, Qt::QueuedConnection);
}

void ReceiptsWidget::reset()
{
	if(!getPlugin<EventPlugin>()->isEventOpen()) {
		m_cardsModel->clearRows();
		return;
	}
	reload();
}

void ReceiptsWidget::reload()
{
	bool is_relays = getPlugin<EventPlugin>()->eventConfig()->isRelays();
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
			.where("cards.stageId=" QF_IARG(current_stage))
			.orderBy("cards.id DESC");
	if(is_relays) {
		qb.join("runs.relayId", "relays.id");
		qb.join("relays.classId", "classes.id");
	}
	else {
		qb.join("competitors.classId", "classes.id");
	}
	m_cardsModel->setQueryBuilder(qb, false);
	m_cardsModel->reload();
}

void ReceiptsWidget::onDbEventNotify(const QString &domain, int connection_id, const QVariant &data)
{
	Q_UNUSED(connection_id)
	Q_UNUSED(data)
	if(domain == QLatin1String(Event::EventPlugin::DBEVENT_CARD_READ)) {
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
	return getPlugin<EventPlugin>()->currentStageId();
}

void ReceiptsWidget::onCardRead()
{
	if(isAutoPrintEnabled()) {
		printNewCards();
	}
	loadNewCards();
}

void ReceiptsWidget::printNewCards()
{
	auto conn  = qf::core::sql::Connection::forName();
	int connection_id = conn.connectionId();
	QF_ASSERT(connection_id > 0, "Cannot get SQL connection id", return);
	int current_stage = currentStageId();
	QString qs = "UPDATE cards SET printerConnectionId=" QF_IARG(connection_id)
			" WHERE printerConnectionId IS NULL"
			" AND cards.stageId=" QF_IARG(current_stage);
	if(ui->chkThisReaderOnly->isChecked()) {
		qs += " AND readerConnectionId=" QF_IARG(connection_id);
	}
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
			if(!printReceipt(card_id))
				break;
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
	QAction a_print_receipts(tr("Print receipts for selected rows"), nullptr);
	QAction a_show_receipt(tr("Show receipt"), nullptr);
	QList<QAction*> lst;
	lst << &a_print_receipts;
	lst << &a_show_receipt;
	QAction *a = QMenu::exec(lst, ui->tblCards->viewport()->mapToGlobal(pos));
	if(a == &a_print_receipts) {
		printSelectedCards();
	}
	else if(a == &a_show_receipt) {
		int card_id = ui->tblCards->selectedRow().value("cards.id").toInt();
		getPlugin<ReceiptsPlugin>()->previewReceipt(card_id);
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
	qf::core::sql::Query q;
	if(q.exec("SELECT runId FROM cards WHERE id=" QF_IARG(card_id))) {
		if(q.next()) {
			int run_id = q.value(0).toInt();
			if(run_id > 0)
				return getPlugin<ReceiptsPlugin>()->printReceipt(card_id);
			else
			{
				if (ui->lstNotFound->currentIndex() == 0)
					return getPlugin<ReceiptsPlugin>()->printError(card_id);
				else
					return getPlugin<ReceiptsPlugin>()->printCard(card_id);
			}
		}
	}
	return false;
}

void ReceiptsWidget::loadReceptList()
{
	ui->lstReceipt->clear();
	QString receipts_dir = getPlugin<ReceiptsPlugin>()->homeDir() + "/reports/receipts";
	QDirIterator it(receipts_dir, QStringList{"*.qml"}, QDir::Files | QDir::Readable, QDirIterator::Subdirectories);
	while (it.hasNext()) {
		it.next();
		QFileInfo fi = it.fileInfo();
		QString path = fi.filePath();
		QString name = path.mid(receipts_dir.length() + 1);
		if(name.startsWith("private/", Qt::CaseInsensitive))
			continue;
		name = name.mid(0, name.indexOf(".qml", Qt::CaseInsensitive));
		qfInfo() << "receipt:" << name << "path:" << path;
		ui->lstReceipt->addItem(name, path);
	}
	ui->lstReceipt->setCurrentIndex(-1);
	QString curr_path = getPlugin<ReceiptsPlugin>()->currentReceiptPath();
	qfInfo() << "current receipt path:" << curr_path;
	for (int i = 0; i < ui->lstReceipt->count(); ++i) {
		if(ui->lstReceipt->itemData(i).toString() == curr_path) {
			ui->lstReceipt->setCurrentIndex(i);
			break;
		}
	}
	if(ui->lstReceipt->currentIndex() < 0) {
		ui->lstReceipt->setCurrentIndex(0);
		getPlugin<ReceiptsPlugin>()->setCurrentReceiptPath(ui->lstReceipt->itemData(0).toString());
	}
}

void ReceiptsWidget::updateReceiptsPrinterLabel()
{
	const auto &opts = getPlugin<ReceiptsPlugin>()->receiptsPrinter()->printerOptions();
	ui->btPrinterOptions->setText(opts.printerCaption());
	if(opts.printerType() == (int)ReceiptsPrinterOptions::PrinterType::GraphicPrinter)
		ui->btPrinterOptions->setIcon(QIcon(":/quickevent/Receipts/images/graphic-printer.svg"));
	else
		ui->btPrinterOptions->setIcon(QIcon(":/quickevent/Receipts/images/character-printer.svg"));
}

void ReceiptsWidget::on_btPrinterOptions_clicked()
{
	ReceiptsPrinterOptionsDialog dlg(this);
	dlg.setPrinterOptions(getPlugin<ReceiptsPlugin>()->receiptsPrinter()->printerOptions());
	if(dlg.exec()) {
		getPlugin<ReceiptsPlugin>()->setReceiptsPrinterOptions(dlg.printerOptions());
		updateReceiptsPrinterLabel();
	}
}

bool ReceiptsWidget::isAutoPrintEnabled()
{
	return ui->chkAutoPrint->isChecked();
}
