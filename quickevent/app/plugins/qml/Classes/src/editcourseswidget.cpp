#include "editcoursecodeswidget.h"
#include "editcourseswidget.h"
#include "ui_editcourseswidget.h"

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
		m->addColumn("code_count", tr("Cnt")).setToolTip(tr("Codes count"));
		m->addColumn("code_list", tr("Codes"));
		ui->tblCourses->setTableModel(m);
		m_coursesModel = m;
	}
	{
		qf::core::sql::Connection conn = m_coursesModel->sqlConnection();
		qfs::QueryBuilder qb_code_count;
		qb_code_count.select("COUNT(*)").from("coursecodes").where("coursecodes.courseId=courses.id").as("code_count");
		qfs::QueryBuilder qb_code_list;
		qb_code_list.from("coursecodes")
				.join("coursecodes.codeId", "codes.id")
				.where("coursecodes.courseId=courses.id")
				.as("code_list");
		if(conn.driverName().endsWith(QLatin1String("PSQL"), Qt::CaseInsensitive)) {
			qb_code_list.select("string_agg(CAST(codes.code as VARCHAR), ',' ORDER BY coursecodes.position)");
		}
		else {
			qb_code_list.select("GROUP_CONCAT(codes.code)")
				.orderBy("coursecodes.position");
		}
		qfs::QueryBuilder qb;
		qb.select2("courses", "*")
				.select(qb_code_list.toString())
				.select(qb_code_count.toString())
				.from("courses")
				.orderBy("courses.name");
		m_coursesModel->setQueryBuilder(qb);
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
