#include "editcoursecodeswidget.h"
#include "ui_editcoursecodeswidget.h"

#include <qf/core/model/sqltablemodel.h>

namespace qfc = qf::core;
namespace qfw = qf::qmlwidgets;
namespace qfd = qf::qmlwidgets::dialogs;
namespace qfm = qf::core::model;
namespace qfs = qf::core::sql;

EditCourseCodesWidget::EditCourseCodesWidget(int course_id, QWidget *parent)
	: Super(parent)
	, ui(new Ui::EditCourseCodesWidget)
	, m_courseId(course_id)
{
	setPersistentSettingsId("EditCourseCodesWidget");
	ui->setupUi(this);
	{
		ui->tblCourseCodes->setPersistentSettingsId("tblCourseCodes");
		ui->tblCourseCodes->setInsertRowEnabled(false);
		ui->tblCourseCodes->setCloneRowEnabled(false);
		ui->tblCourseCodes->setRemoveRowEnabled(false);
		ui->tblCourseCodesTB->setTableView(ui->tblCourseCodes);
		qfm::SqlTableModel *m = new qfm::SqlTableModel(this);
		//m->addColumn("id").setReadOnly(true);
		m->addColumn("coursecodes.position", tr("Code"));
		m->addColumn("codes.code", tr("Code"));
		m->addColumn("codes.note", tr("Note"));
		ui->tblCourseCodes->setTableModel(m);
		m_courseCodesModel = m;
	}
	{
		qfs::QueryBuilder qb;
		qb.select2("codes", "code, note")
				.select2("coursecodes", "position")
				.from("coursecodes")
				.joinRestricted("coursecodes.codeId", "codes.id", "coursecodes.courseId=" QF_IARG(m_courseId), qfs::QueryBuilder::INNER_JOIN)
				.orderBy("coursecodes.position");
		qfInfo() << qb.toString();
		m_courseCodesModel->setQueryBuilder(qb);
		m_courseCodesModel->reload();
	}
}

EditCourseCodesWidget::~EditCourseCodesWidget()
{
	delete ui;
}
