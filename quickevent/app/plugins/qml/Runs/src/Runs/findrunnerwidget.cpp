#include "findrunnerwidget.h"
#include "ui_findrunnerwidget.h"

#include <qf/core/model/sqltablemodel.h>
#include <qf/core/sql/querybuilder.h>

namespace qfm = qf::core::model;
namespace qfs = qf::core::sql;

namespace Runs {

FindRunnerWidget::FindRunnerWidget(int stage_id, QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::FindRunnerWidget)
	, m_stageId(stage_id)
{
	ui->setupUi(this);

	ui->tblRunnersTB->setTableView(ui->tblRunners);
	ui->tblRunnersTB->hide();

	ui->tblRunners->setPersistentSettingsId(ui->tblRunners->objectName());
	ui->tblRunners->setReadOnly(true);
	qfm::SqlTableModel *m = new qfm::SqlTableModel(this);
	m->addColumn("competitorName", tr("Name"));
	m->addColumn("classes.name", tr("Class"));
	m->addColumn("registration", tr("Reg"));
	m->addColumn("siId", tr("SI"));
	ui->tblRunners->setTableModel(m);
	m_runnersModel = m;

	connect(ui->edNameFilter, &QLineEdit::textChanged, this, &FindRunnerWidget::reload);
	connect(ui->edClassFilter, &QLineEdit::textChanged, this, &FindRunnerWidget::reload);
	connect(ui->edRegistrationFilter, &QLineEdit::textChanged, this, &FindRunnerWidget::reload);
	connect(ui->edSiIdFilter, &QLineEdit::textChanged, this, &FindRunnerWidget::reload);
}

FindRunnerWidget::~FindRunnerWidget()
{
	delete ui;
}

void FindRunnerWidget::reload()
{
	qfLogFuncFrame();
	if(!isVisible())
		return;
	qfs::QueryBuilder qb;
	qb.select2("competitors", "registration")
			.select("COALESCE(lastName, '') || ' ' || COALESCE(firstName, '') AS competitorName")
			.select2("runs", "id, siId")
			.select2("classes", "name")
			.from("competitors")
			.join("competitors.classId", "classes.id")
			.joinRestricted("competitors.id", "runs.competitorId", "runs.stageId=" QF_IARG(m_stageId), "JOIN")
			.orderBy("classes.name, lastName, firstName");
	{
		QString first_name_filter;
		QString last_name_filter = ui->edNameFilter->text().trimmed().toLower();
		int ix = last_name_filter.indexOf(' ');
		if(ix > 0) {
			first_name_filter = last_name_filter.mid(ix + 1).trimmed();
			last_name_filter = last_name_filter.mid(0, ix).trimmed();
		}
		QString class_filter = ui->edClassFilter->text().trimmed().toLower();
		QString registration_filter = ui->edRegistrationFilter->text().trimmed().toLower();
		QString siid_filter = ui->edSiIdFilter->text().trimmed();

		int l = first_name_filter.length()
				+ last_name_filter.length()
				+ class_filter.length()
				+ registration_filter.length()
				+ siid_filter.length();
		if(l < 3)
			return;
		qfDebug() << "first_name:" << first_name_filter << "last_name:" << last_name_filter;
		if(!first_name_filter.isEmpty())
			qb.where("LOWER(firstName) LIKE '%" + first_name_filter + "%'");
		if(!last_name_filter.isEmpty())
			qb.where("LOWER(lastName) LIKE '%" + last_name_filter + "%'");
		if(!class_filter.isEmpty())
			qb.where("LOWER(classes.name) LIKE '%" + class_filter + "%'");
		if(!registration_filter.isEmpty())
			qb.where("LOWER(registration) LIKE '%" + registration_filter + "%'");
		if(!siid_filter.isEmpty())
			qb.where("runs.siId LIKE '%" + siid_filter + "%'");
	}
	m_runnersModel->setQueryBuilder(qb);
	m_runnersModel->reload();
	ui->tblRunners->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);
}

void FindRunnerWidget::setFocusToWidget(FindRunnerWidget::FocusWidget fw)
{
	switch (fw) {
	case FocusWidget::Name:
		ui->edNameFilter->setFocus();
		break;
	case FocusWidget::Registration:
		ui->edRegistrationFilter->setFocus();
		break;
	default:
		break;
	}
}

qf::qmlwidgets::TableView *FindRunnerWidget::tableView()
{
	return ui->tblRunners;
}

}
