#ifndef EDITCODESWIDGET_H
#define EDITCODESWIDGET_H

#include <qf/qmlwidgets/framework/dialogwidget.h>

namespace Ui {
class EditCodesWidget;
}

namespace qf {
namespace core { namespace model { class SqlTableModel; } }
}

class EditCodesWidget : public qf::qmlwidgets::framework::DialogWidget
{
	Q_OBJECT
private:
	typedef qf::qmlwidgets::framework::DialogWidget Super;
public:
	explicit EditCodesWidget(QWidget *parent = 0);
	~EditCodesWidget() Q_DECL_OVERRIDE;
private:
	Ui::EditCodesWidget *ui;
	qf::core::model::SqlTableModel *m_tableModel;
};

#endif // EDITCODESWIDGET_H
