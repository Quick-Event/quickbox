#include "registrationswidget.h"
#include "ui_registrationswidget.h"

#include <qf/core/model/sqltablemodel.h>
#include <qf/core/sql/querybuilder.h>
#include <qf/core/sql/connection.h>

namespace qfm = qf::core::model;
namespace qfs = qf::core::sql;

RegistrationsWidget::RegistrationsWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::RegistrationsWidget)
{
	ui->setupUi(this);

	ui->tblRegistrationsTB->setTableView(ui->tblRegistrations);

	ui->tblRegistrations->setPersistentSettingsId("tblRegistrations");
	ui->tblRegistrations->setReadOnly(true);
	//ui->tblRegistrations->setInlineEditStrategy(qfw::TableView::OnCurrentFieldChange);
	qfm::SqlTableModel *m = new qfm::SqlTableModel(this);
	m->addColumn("competitorName", tr("Name"));
	m->addColumn("registration", tr("Reg"));
	m->addColumn("siId", tr("SI"));
	ui->tblRegistrations->setTableModel(m);
	m_registrationsModel = m;

	connect(ui->edNameFilter, &QLineEdit::textChanged, this, &RegistrationsWidget::reload);
	connect(ui->edRegistrationFilter, &QLineEdit::textChanged, this, &RegistrationsWidget::reload);
	connect(ui->edSiIdFilter, &QLineEdit::textChanged, this, &RegistrationsWidget::reload);
	connect(ui->grpFilter, &QGroupBox::toggled, this, &RegistrationsWidget::reload);
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
	qfs::QueryBuilder qb;
	qb.select2("registrations", "firstName, lastName, licence, registration, siId")
			.select("COALESCE(lastName, '') || ' ' || COALESCE(firstName, '') AS competitorName")
			.from("registrations")
			.orderBy("registration");
	if(ui->grpFilter->isChecked()) {
		QString name_filter = ui->edNameFilter->text().trimmed().toLower();
		QString registration_filter = ui->edRegistrationFilter->text().trimmed().toLower();
		QString siid_filter = ui->edSiIdFilter->text().trimmed();

		int l = name_filter.length()
				+ registration_filter.length()
				+ siid_filter.length();
		if(l < 3)
			return;
		qfDebug() << "name:" << name_filter;
		if(!name_filter.isEmpty())
			qb.where("nameSearchKey LIKE '" + name_filter + "%'");
		if(!registration_filter.isEmpty())
			qb.where("LOWER(registration) LIKE '%" + registration_filter + "%'");
		if(!siid_filter.isEmpty()) {
			// this works for sqlite
			// postgres doesn't allow to test integer using LIKE
			qb.where("'' || siId LIKE '%" + siid_filter + "%'");
		}
	}
	m_registrationsModel->setQueryBuilder(qb);
	m_registrationsModel->reload();
	ui->tblRegistrations->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);
}

void RegistrationsWidget::onDbEvent(const QString &domain, const QVariant &payload)
{
	qfLogFuncFrame() << "domain:" << domain << "payload:" << payload;
	if(domain == "Oris.registrationImported")
		reload();
}

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

qf::qmlwidgets::TableView *RegistrationsWidget::tableView()
{
	return ui->tblRegistrations;
}

