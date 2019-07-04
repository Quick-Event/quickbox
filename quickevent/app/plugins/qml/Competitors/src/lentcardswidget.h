#pragma once

#include <qf/qmlwidgets/framework/dialogwidget.h>

namespace Ui {
class LentCardsWidget;
}

namespace qf {
namespace core { namespace model { class SqlTableModel; } }
}

class LentCardsWidget : public qf::qmlwidgets::framework::DialogWidget
{
	Q_OBJECT
private:
	using Super = qf::qmlwidgets::framework::DialogWidget;

public:
	explicit LentCardsWidget(QWidget *parent = 0);
	~LentCardsWidget() Q_DECL_OVERRIDE;

private:
	Ui::LentCardsWidget *ui;
	qf::core::model::SqlTableModel *m_tableModel;
};

