#ifndef EDITCOURSESWIDGET_H
#define EDITCOURSESWIDGET_H

#include <qf/qmlwidgets/framework/dialogwidget.h>

namespace Ui {
class EditCoursesWidget;
}

namespace qf {
namespace core { namespace model { class SqlTableModel; } }
}

class EditCoursesWidget : public qf::qmlwidgets::framework::DialogWidget
{
	Q_OBJECT
private:
	typedef qf::qmlwidgets::framework::DialogWidget Super;
public:
	explicit EditCoursesWidget(QWidget *parent = 0);
	~EditCoursesWidget() Q_DECL_OVERRIDE;

	void editCourseCodes(const QModelIndex &ix);
private:
	Ui::EditCoursesWidget *ui;
	qf::core::model::SqlTableModel *m_coursesModel;
};

#endif // EDITCOURSESWIDGET_H
