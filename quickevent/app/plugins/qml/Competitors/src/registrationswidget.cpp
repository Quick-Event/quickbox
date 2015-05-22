#include "registrationswidget.h"
#include "ui_registrationswidget.h"

#include <qf/core/model/sqltablemodel.h>
#include <qf/core/sql/querybuilder.h>

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

	connect(ui->edNameFilter, &QLineEdit::textChanged, this, &RegistrationsWidget::onFilterTextChanged);
	connect(ui->edRegistrationFilter, &QLineEdit::textChanged, this, &RegistrationsWidget::onFilterTextChanged);
	connect(ui->edSiIdFilter, &QLineEdit::textChanged, this, &RegistrationsWidget::onFilterTextChanged);
	connect(ui->grpFilter, &QGroupBox::toggled, this, &RegistrationsWidget::onGrpFilterToggled);
}

RegistrationsWidget::~RegistrationsWidget()
{
	delete ui;
}

void RegistrationsWidget::reload()
{
	qfs::QueryBuilder qb;
	qb.select2("registrations", "registration, siId")
			.select("COALESCE(lastName, '') || ' ' || COALESCE(firstName, '') AS competitorName")
			.from("registrations")
			.orderBy("registration");
	if(ui->grpFilter->isChecked()) {
		QString first_name_filter;
		QString last_name_filter = ui->edNameFilter->text().trimmed();
		int ix = last_name_filter.indexOf(' ');
		if(ix > 0) {
			last_name_filter = last_name_filter.mid(0, ix).trimmed();
			first_name_filter = last_name_filter.mid(ix + 1).trimmed();
		}
		QString registration_filter = ui->edRegistrationFilter->text().trimmed();
		QString siid_filter = ui->edSiIdFilter->text().trimmed();
		if(!first_name_filter.isEmpty())
			qb.where("firstName LIKE '%" + first_name_filter + "%'");
		if(!last_name_filter.isEmpty())
			qb.where("lastName LIKE '%" + last_name_filter + "%'");
		if(!registration_filter.isEmpty())
			qb.where("registration LIKE '%" + registration_filter + "%'");
		if(!siid_filter.isEmpty())
			qb.where("siId LIKE '%" + siid_filter + "%'");
	}
	m_registrationsModel->setQueryBuilder(qb);
	m_registrationsModel->reload();
	ui->tblRegistrations->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);
}

void RegistrationsWidget::onFilterTextChanged()
{
	if(ui->grpFilter->isChecked()) {
		int l = ui->edNameFilter->text().trimmed().length()
				+ ui->edRegistrationFilter->text().trimmed().length()
				+ ui->edSiIdFilter->text().trimmed().length();
		if(l >= 3)
			reload();
	}
}

void RegistrationsWidget::onGrpFilterToggled()
{
	if(!ui->grpFilter->isChecked()) {
		reload();
	}
	else {
		onFilterTextChanged();
	}
}
