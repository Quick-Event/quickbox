#include "editcoursecodeswidget.h"
#include "ui_editcoursecodeswidget.h"

#include <qf/core/model/sqltablemodel.h>

#include <QStandardItemModel>
#include <QDialogButtonBox>

#include <qf/qmlwidgets/dialogs/messagebox.h>

#include <qf/core/sql/transaction.h>
#include <qf/core/assert.h>

namespace qfc = qf::core;
namespace qfw = qf::qmlwidgets;
namespace qfd = qf::qmlwidgets::dialogs;
namespace qfm = qf::core::model;
namespace qfs = qf::core::sql;

EditCourseCodesWidget::EditCourseCodesWidget(QWidget *parent)
	: Super(parent)
	, ui(new Ui::EditCourseCodesWidget)
{
	setPersistentSettingsId("EditCourseCodesWidget");
	ui->setupUi(this);

	ui->tblCourseCodes->verticalHeader()->setDefaultSectionSize((int)(fontMetrics().lineSpacing() * 1.3));
	ui->tblAllCodes->verticalHeader()->setDefaultSectionSize((int)(fontMetrics().lineSpacing() * 1.3));

	m_courseCodesModel = new QStandardItemModel(this);
	ui->tblCourseCodes->setModel(m_courseCodesModel);

	m_allCodesModel = new QStandardItemModel(this);
	ui->tblAllCodes->setModel(m_allCodesModel);

	connect(ui->btAddCodes, &QPushButton::clicked, this, &EditCourseCodesWidget::addSelectedCodesToCourse);
	connect(ui->btRemoveCodes, &QPushButton::clicked, this, &EditCourseCodesWidget::removeSelectedCodesFromCourse);
	connect(ui->btCodesUp, &QPushButton::clicked, this, &EditCourseCodesWidget::moveSelectedCodesUp);
	connect(ui->btCodesDown, &QPushButton::clicked, this, &EditCourseCodesWidget::moveSelectedCodesDown);
}

EditCourseCodesWidget::~EditCourseCodesWidget()
{
	delete ui;
}

void EditCourseCodesWidget::reload(int course_id)
{
	qfLogFuncFrame() << course_id;
	if(course_id > 0)
		m_courseId = course_id;
	QF_ASSERT(m_courseId > 0, "Bad course ID", return);
	loadAllCodes();
	auto *m = m_courseCodesModel;
	m->clear();
	m->setHorizontalHeaderLabels(QStringList() << tr("Code"));
	qfs::QueryBuilder qb;
	qb.select2("codes", "id, code, note")
			//.select2("coursecodes", "position")
			.from("coursecodes")
			.joinRestricted("coursecodes.codeId", "codes.id", "coursecodes.courseId=" QF_IARG(m_courseId), qfs::QueryBuilder::INNER_JOIN)
			.orderBy("coursecodes.position");
	//qfInfo() << qb.toString();
	qfs::Query q;
	q.exec(qb);
	while(q.next()) {
		int code_id = q.value(QStringLiteral("id")).toInt();
		addCourseCode(code_id);
	}
}

void EditCourseCodesWidget::save()
{
	qfLogFuncFrame();
	try {
		qfs::Transaction transaction;
		qfs::Query q;
		q.exec("DELETE FROM coursecodes WHERE courseId=" QF_IARG(m_courseId), qfc::Exception::Throw);
		q.prepare("INSERT INTO coursecodes (courseId, position, codeId) VALUES (:courseId, :position, :codeId)", qfc::Exception::Throw);
		for (int i = 0; i < m_courseCodesModel->rowCount(); ++i) {
			q.bindValue(QStringLiteral(":position"), i + 1);
			q.bindValue(QStringLiteral(":courseId"), m_courseId);
			q.bindValue(QStringLiteral(":codeId"), m_courseCodesModel->item(i)->data().toInt());
			q.exec(qfc::Exception::Throw);
		}
		transaction.commit();
		emit courseCodesSaved();
	}
	catch (qfc::Exception &ex) {
		qfd::MessageBox::showException(this, ex);
	}
	m_edited = false;
}

bool EditCourseCodesWidget::acceptDialogDone(int result)
{
	qfLogFuncFrame() << "result:" << result;
	if(result && m_edited && qf::qmlwidgets::dialogs::MessageBox::askYesNo(this, tr("Save changes?"))) {
		save();
	}
	return true;
}

void EditCourseCodesWidget::loadAllCodes()
{
	auto *m = m_allCodesModel;
	m->clear();
	m->setHorizontalHeaderLabels(QStringList() << tr("Code"));
	qfs::QueryBuilder qb;
	qb.select2("codes", "id, code, note")
			.from("codes")
			.orderBy("codes.code");
	//qfInfo() << qb.toString();
	qfs::Query q;
	q.exec(qb);
	while(q.next()) {
		auto caption = QString("%1 %2").arg(q.value(QStringLiteral("code")).toInt()).arg(q.value(QStringLiteral("note")).toString());
		int code_id = q.value(QStringLiteral("id")).toInt();
		QStandardItem *it = new QStandardItem(caption);
		it->setData(code_id);
		m->appendRow(it);
	}
}

void EditCourseCodesWidget::addCourseCode(int code_id)
{
	for (int i = 0; i < m_allCodesModel->rowCount(); ++i) {
		QStandardItem *it1 = m_allCodesModel->item(i);
		int id = it1->data().toInt();
		if(id == code_id) {
			QStandardItem *it2 = new QStandardItem(it1->text());
			it2->setData(it1->data());
			//m_allCodesModel->removeRow(i);
			m_courseCodesModel->appendRow(it2);
			break;
		}
	}
}

void EditCourseCodesWidget::addSelectedCodesToCourse()
{
	m_edited = true;
	QList<int> selected_codes;
	for(auto ix : ui->tblAllCodes->selectionModel()->selectedRows()) {
		int code_id = ix.data(Qt::UserRole + 1).toInt();
		selected_codes << code_id;
	}
	for(auto code_id : selected_codes) {
		addCourseCode(code_id);
	}
}

void EditCourseCodesWidget::removeSelectedCodesFromCourse()
{
	m_edited = true;
	QList<int> selected_codes;
	for(auto ix : ui->tblCourseCodes->selectionModel()->selectedRows()) {
		int code_id = ix.data(Qt::UserRole + 1).toInt();
		selected_codes << code_id;
	}
	for(auto code_id : selected_codes) {
		for (int i = 0; i < m_courseCodesModel->rowCount(); ++i) {
			int id = m_courseCodesModel->item(i)->data().toInt();
			if(id == code_id) {
				m_courseCodesModel->removeRow(i);
				break;
			}
		}
	}
}

void EditCourseCodesWidget::switchWithPrevCourseCode(int row_no)
{
	QStandardItemModel *m = m_courseCodesModel;
	if(row_no > 0 && row_no < m->rowCount()) {
		QStandardItem *it1 = m->item(row_no - 1);
		QStandardItem *it2 = m->item(row_no);
		QString text1 = it1->text();
		QVariant data1 = it1->data();
		it1->setText(it2->text());
		it1->setData(it2->data());
		it2->setText(text1);
		it2->setData(data1);
	}
}

void EditCourseCodesWidget::switchWithNextCourseCode(int row_no)
{
	QStandardItemModel *m = m_courseCodesModel;
	if(row_no >= 0 && row_no < m->rowCount() - 1) {
		QStandardItem *it1 = m->item(row_no);
		QStandardItem *it2 = m->item(row_no + 1);
		QString text1 = it1->text();
		QVariant data1 = it1->data();
		it1->setText(it2->text());
		it1->setData(it2->data());
		it2->setText(text1);
		it2->setData(data1);
	}
}

void EditCourseCodesWidget::moveSelectedCodesUp()
{
	m_edited = true;
	QModelIndexList selected_rows = ui->tblCourseCodes->selectionModel()->selectedRows();
	for(const auto &ix : selected_rows) {
		int row_no = ix.row();
		switchWithPrevCourseCode(row_no);
	}
	QItemSelectionModel *sm = ui->tblCourseCodes->selectionModel();
	sm->clearSelection();
	for(const auto &ix : selected_rows) {
		QModelIndex ix2 = ix.sibling(ix.row() - 1, 0);
		sm->select(ix2, QItemSelectionModel::SelectionFlag::Select);
	}
}

void EditCourseCodesWidget::moveSelectedCodesDown()
{
	m_edited = true;
	QModelIndexList selected_rows = ui->tblCourseCodes->selectionModel()->selectedRows();
	for(const auto &ix : selected_rows) {
		int row_no = ix.row();
		switchWithNextCourseCode(row_no);
	}
	QItemSelectionModel *sm = ui->tblCourseCodes->selectionModel();
	sm->clearSelection();
	for(const auto &ix : selected_rows) {
		QModelIndex ix2 = ix.sibling(ix.row() + 1, 0);
		sm->select(ix2, QItemSelectionModel::SelectionFlag::Select);
	}
}

