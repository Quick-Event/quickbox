#ifndef EDITCOURSECODESWIDGET_H
#define EDITCOURSECODESWIDGET_H

#include <qf/qmlwidgets/framework/dialogwidget.h>

class QStandardItemModel;

namespace Ui {
class EditCourseCodesWidget;
}

namespace qf {
namespace core { namespace model { class SqlTableModel; } }
}

class EditCourseCodesWidget : public qf::qmlwidgets::framework::DialogWidget
{
	Q_OBJECT
private:
	typedef qf::qmlwidgets::framework::DialogWidget Super;
public:
	explicit EditCourseCodesWidget(QWidget *parent = 0);
	~EditCourseCodesWidget() Q_DECL_OVERRIDE;

	void reload(int course_id = 0);
	void save();

	bool acceptDialogDone(int result) Q_DECL_OVERRIDE;

	Q_SIGNAL void courseCodesSaved();
private:
	void loadAllCodes();
	void addCourseCode(int code_id);
	void addSelectedCodesToCourse();
	void removeSelectedCodesFromCourse();

	void moveSelectedCodesUp();
	void switchWithPrevCourseCode(int row_no);

	void moveSelectedCodesDown();
	void switchWithNextCourseCode(int row_no);
private:
	Ui::EditCourseCodesWidget *ui;
	QStandardItemModel *m_courseCodesModel;
	QStandardItemModel *m_allCodesModel;
	int m_courseId;
	bool m_edited = false;
};

#endif // EDITCOURSECODESWIDGET_H
