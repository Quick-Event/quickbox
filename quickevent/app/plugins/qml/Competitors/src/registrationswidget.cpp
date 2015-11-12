#include "registrationswidget.h"
#include "ui_registrationswidget.h"
#include "Competitors/competitorsplugin.h"

#include <qf/qmlwidgets/framework/mainwindow.h>

#include <qf/core/model/sqltablemodel.h>
#include <qf/core/sql/querybuilder.h>
#include <qf/core/sql/connection.h>

namespace qfm = qf::core::model;
namespace qfs = qf::core::sql;

static Competitors::CompetitorsPlugin* thisPlugin()
{
	qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
	auto plugin = qobject_cast<Competitors::CompetitorsPlugin *>(fwk->plugin("Competitors"));
	QF_ASSERT_EX(plugin != nullptr, "Bad plugin");
	return plugin;
}

RegistrationsWidget::RegistrationsWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::RegistrationsWidget)
{
	ui->setupUi(this);

	ui->tblRegistrationsTB->setTableView(ui->tblRegistrations);

	ui->tblRegistrations->setPersistentSettingsId("tblRegistrations");
	ui->tblRegistrations->setReadOnly(true);
	//ui->tblRegistrations->setInlineEditStrategy(qfw::TableView::OnCurrentFieldChange);
}

RegistrationsWidget::~RegistrationsWidget()
{
	delete ui;
}

void RegistrationsWidget::reload()
{
	qfLogFuncFrame();
	if(!isVisible())
		return;

	if(!ui->tblRegistrations->tableModel()) {
		ui->tblRegistrations->setTableModel(thisPlugin()->registrationsModel());
		ui->tblRegistrations->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);
	}
}

void RegistrationsWidget::onDbEvent(const QString &domain, const QVariant &payload)
{
	qfLogFuncFrame() << "domain:" << domain << "payload:" << payload;
	if(domain == "Oris.registrationImported")
		reload();
}
/*
void RegistrationsWidget::setFocusToWidget(RegistrationsWidget::FocusWidget fw)
{
	switch (fw) {
	case FocusWidget::Registration:
		ui->edRegistrationFilter->setFocus();
		break;
	default:
		break;
	}
}
*/
qf::qmlwidgets::TableView *RegistrationsWidget::tableView()
{
	return ui->tblRegistrations;
}

