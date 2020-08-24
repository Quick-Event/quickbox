#include "editcoursecodeswidget.h"
#include "editcourseswidget.h"
#include "ui_editcourseswidget.h"

#include <quickevent/core/codedef.h>
#include <qf/core/model/sqltablemodel.h>
#include <qf/core/sql/connection.h>
#include <qf/qmlwidgets/dialogs/dialog.h>
#include <qf/qmlwidgets/dialogbuttonbox.h>
#include <QPushButton>

namespace qfc = qf::core;
namespace qfw = qf::qmlwidgets;
namespace qfd = qf::qmlwidgets::dialogs;
namespace qfm = qf::core::model;
namespace qfs = qf::core::sql;

EditCoursesWidget::EditCoursesWidget(QWidget *parent)
	: Super(parent)
	, ui(new Ui::EditCoursesWidget)
{
	setPersistentSettingsId("EditCoursesWidget");
	ui->setupUi(this);
	{
		ui->tblCourses->setPersistentSettingsId("tblCourses");
		ui->tblCoursesTB->setTableView(ui->tblCourses);
		ui->tblCourses->setRowEditorMode(qfw::TableView::RowEditorMode::EditRowsInline);
		connect(ui->tblCourses, &qfw::TableView::editCellRequest, [this](const QModelIndex &ix) {
			if(ix.column() == this->m_coursesModel->columnIndex("code_list")) {
				editCourseCodes(ix);
			}
		});
		qfm::SqlTableModel *m = new qfm::SqlTableModel(this);
		m->addColumn("id").setReadOnly(true);
		m->addColumn("courses.name", tr("Name"));
		m->addColumn("courses.length", tr("Length"));
		m->addColumn("courses.climb", tr("Climb"));
		m->addColumn("courses.note", tr("Note"));
		m->addColumn("code_count", tr("Cnt")).setToolTip(tr("Control count"));
		m->addColumn("code_list", tr("Codes"));
		ui->tblCourses->setTableModel(m);
		m_coursesModel = m;
	}
	{
		qf::core::sql::Connection conn = m_coursesModel->sqlConnection();
		qfs::QueryBuilder qb_code_count;
		qb_code_count.select("COUNT(*)").from("coursecodes")
				.join("coursecodes.codeId", "codes.id")
				.where("coursecodes.courseId=courses.id AND "
						"codes.code >= " QF_IARG(quickevent::core::CodeDef::PUNCH_CODE_MIN) " AND "
						"codes.code <= " QF_IARG(quickevent::core::CodeDef::PUNCH_CODE_MAX))
				.as("code_count");

		QString control_code_query = "SELECT CAST(code AS TEXT) AS code, position"
										" FROM coursecodes INNER JOIN codes ON codes.id = coursecodes.codeId WHERE (coursecodes.courseId = courses.id)"
										" AND code >= " QF_IARG(quickevent::core::CodeDef::PUNCH_CODE_MIN)
										" AND code <= " QF_IARG(quickevent::core::CodeDef::PUNCH_CODE_MAX);
		QString start_code_query = "SELECT 'S' || (code - " QF_IARG(quickevent::core::CodeDef::START_PUNCH_CODE) " + 1) AS code, position"
										" FROM coursecodes INNER JOIN codes ON codes.id = coursecodes.codeId WHERE (coursecodes.courseId = courses.id)"
										" AND code >= " QF_IARG(quickevent::core::CodeDef::START_PUNCH_CODE)
										" AND code < " QF_IARG(quickevent::core::CodeDef::PUNCH_CODE_MIN);
		QString finnish_code_query = "SELECT 'F' || (code - " QF_IARG(quickevent::core::CodeDef::FINISH_PUNCH_CODE) " + 1) AS code, position "
										" FROM coursecodes INNER JOIN codes ON codes.id = coursecodes.codeId WHERE (coursecodes.courseId = courses.id)"
										" AND code >= " QF_IARG(quickevent::core::CodeDef::FINISH_PUNCH_CODE);

		QString code_list_query = start_code_query + " UNION " + control_code_query + " UNION " + finnish_code_query + " ORDER BY position";
		if(conn.driverName().endsWith(QLatin1String("PSQL"), Qt::CaseInsensitive)) {
			code_list_query = "(SELECT string_agg(code, ',') FROM (" + code_list_query + ") AS code_list_query )";
		}
		else {
			code_list_query = "(SELECT GROUP_CONCAT(code) FROM (" + code_list_query + ") )";
		}

		qfs::QueryBuilder qb;
		qb.select2("courses", "*")
				.select(code_list_query + "AS code_list")
				.select(qb_code_count.toString())
				.from("courses")
				.orderBy("courses.name");
		m_coursesModel->setQueryBuilder(qb, false);
		m_coursesModel->reload();
	}
}

EditCoursesWidget::~EditCoursesWidget()
{
	delete ui;
}

void EditCoursesWidget::editCourseCodes(const QModelIndex &ix)
{
	qfc::model::TableModel *m = ui->tblCourses->tableModel();
	if(!m)
		return;
	int row_no = ui->tblCourses->toTableModelRowNo(ix.row());
	if(row_no < 0)
		return;
	int course_id = m->tableRow(row_no).value("courses.id").toInt();
	if(course_id <= 0)
		return;
	qf::qmlwidgets::dialogs::Dialog dlg(QDialogButtonBox::Close | QDialogButtonBox::Ok | QDialogButtonBox::Apply | QDialogButtonBox::Reset, this);
	auto *w = new EditCourseCodesWidget();
	connect(w, &EditCourseCodesWidget::courseCodesSaved, ui->tblCourses, &qfw::TableView::reloadCurrentRow);
	{
		QPushButton *bt = dlg.buttonBox()->button(QDialogButtonBox::Apply);
		connect(bt, &QPushButton::clicked, w, &EditCourseCodesWidget::save);
	}
	{
		QPushButton *bt = dlg.buttonBox()->button(QDialogButtonBox::Reset);
		connect(bt, &QPushButton::clicked, w, &EditCourseCodesWidget::reload);
	}
	w->reload(course_id);
	dlg.setCentralWidget(w);
	dlg.exec();
}
