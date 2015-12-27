#ifndef EDITCOURSECODESWIDGET_H
#define EDITCOURSECODESWIDGET_H

#include <qf/qmlwidgets/framework/dialogwidget.h>

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
	explicit EditCourseCodesWidget(int course_id, QWidget *parent = 0);
	~EditCourseCodesWidget() Q_DECL_OVERRIDE;
private:
	Ui::EditCourseCodesWidget *ui;
	qf::core::model::SqlTableModel *m_courseCodesModel;
	int m_courseId;
};

#endif // EDITCOURSECODESWIDGET_H
