#include "recipeswidget.h"
#include "ui_recipeswidget.h"
#include "recipespartwidget.h"
#include "Recipes/recipesplugin.h"

#include <Event/eventplugin.h>
#include <CardReader/cardreaderplugin.h>

#include <quickevent/og/timems.h>
#include <quickevent/og/sqltablemodel.h>
#include <quickevent/og/itemdelegate.h>

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

namespace qfm = qf::core::model;
namespace qfs = qf::core::sql;
namespace qff = qf::qmlwidgets::framework;
namespace qfw = qf::qmlwidgets;

const char *RecipesWidget::SETTINGS_PREFIX = "plugins/Recipes";

RecipesWidget::RecipesWidget(QWidget *parent) :
	Super(parent),
	ui(new Ui::RecipesWidget)
{
	ui->setupUi(this);

	createActions();

	loadPrinters();

	{
		ui->tblPrintJobsTB->setTableView(ui->tblCards);

		ui->tblCards->setPersistentSettingsId(ui->tblCards->objectName());
		//ui->tblPrintJobs->setRowEditorMode(qfw::TableView::EditRowsMixed);
		ui->tblCards->setInlineEditSaveStrategy(qfw::TableView::OnEditedValueCommit);
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
		m->addColumn("cards.printerConnectionId", tr("printer"));

		ui->tblCards->setTableModel(m);
		m_cardsModel = m;
	}

	ui->tblCards->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(ui->tblCards, &qfw::TableView::customContextMenuRequested, this, &RecipesWidget::onCustomContextMenuRequest);
}

RecipesWidget::~RecipesWidget()
{
	delete ui;
}

void RecipesWidget::settleDownInPartWidget(RecipesPartWidget *part_widget)
{
	connect(part_widget, SIGNAL(resetPartRequest()), this, SLOT(reset()));
	connect(part_widget, SIGNAL(reloadPartRequest()), this, SLOT(reset()));

	connect(eventPlugin(), SIGNAL(dbEventNotify(QString,QVariant)), this, SLOT(onDbEventNotify(QString,QVariant)), Qt::QueuedConnection);
}

void RecipesWidget::reload()
{
	int current_stage = currentStageId();
	qfs::QueryBuilder qb;
	qb.select2("cards", "id, siId, printerConnectionId")
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

Recipes::RecipesPlugin *RecipesWidget::recipesPlugin()
{
	qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
	auto plugin = qobject_cast<Recipes::RecipesPlugin *>(fwk->plugin("Receipes"));
	QF_ASSERT(plugin != nullptr, "Bad plugin", return nullptr);
	return plugin;
}

Event::EventPlugin *RecipesWidget::eventPlugin()
{
	qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
	auto plugin = qobject_cast<Event::EventPlugin *>(fwk->plugin("Event"));
	QF_ASSERT(plugin != nullptr, "Bad plugin", return nullptr);
	return plugin;
}

void RecipesWidget::onDbEventNotify(const QString &domain, const QVariant &payload)
{
	Q_UNUSED(payload)
	if(domain == QLatin1String(CardReader::CardReaderPlugin::DBEVENTDOMAIN_CARDREADER_CARDREAD)) {
		onCardRead();
	}
}

void RecipesWidget::createActions()
{
	//QStyle *sty = style();
	/*
	{
		QIcon ico(":/quickevent/Receipes/images/comm");
		qf::qmlwidgets::Action *a = new qf::qmlwidgets::Action(ico, tr("Open COM"), this);
		a->setCheckable(true);
		connect(a, SIGNAL(triggered(bool)), this, SLOT(onCommOpen(bool)));
		m_actCommOpen = a;
	}
	*/
}

void RecipesWidget::loadPrinters()
{
	ui->cbxPrinters->addItems(QPrinterInfo::availablePrinterNames());
	QString def = QPrinterInfo::defaultPrinterName();
	ui->cbxPrinters->setCurrentText(def);
}

QPrinterInfo RecipesWidget::currentPrinter()
{
	return QPrinterInfo::printerInfo(ui->cbxPrinters->currentText());
}

int RecipesWidget::currentStageId()
{
	auto event_plugin = eventPlugin();
	QF_ASSERT(event_plugin != nullptr, "Bad plugin", return 0);
	int ret = event_plugin->currentStageId();
	return ret;
}

void RecipesWidget::onCardRead()
{
	if(ui->chkAutoPrint->isChecked()) {
		printNewCards();
	}
	loadNewCards();
}

void RecipesWidget::printNewCards()
{
	auto conn  = qf::core::sql::Connection::forName();
	int connection_id = conn.connectionId();
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
			if(!printRecipe(card_id)) {
				break;
			}
		}
	}
}

void RecipesWidget::loadNewCards()
{
	m_cardsModel->reloadInserts(QStringLiteral("cards.id"));
}

void RecipesWidget::on_btPrintNew_clicked()
{
	printNewCards();
	loadNewCards();
}

void RecipesWidget::onCustomContextMenuRequest(const QPoint &pos)
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

void RecipesWidget::printSelectedCards()
{
	for(int i : ui->tblCards->selectedRowsIndexes()) {
		int card_id = ui->tblCards->tableRow(i).value("cards.id").toInt();
		QF_ASSERT(card_id > 0, "Bad card ID", return);
		printRecipe(card_id);
	}
}

bool RecipesWidget::printRecipe(int card_id)
{
	return recipesPlugin()->printRecipe(card_id, currentPrinter());
}

