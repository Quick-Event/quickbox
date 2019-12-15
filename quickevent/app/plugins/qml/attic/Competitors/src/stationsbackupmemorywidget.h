#pragma once

#include <qf/qmlwidgets/framework/dialogwidget.h>

namespace Ui {
class StationsBackupMemoryWidget;
}

namespace qf {
namespace core { namespace model { class SqlTableModel; } }
}

class StationsBackupMemoryWidget : public qf::qmlwidgets::framework::DialogWidget
{
	Q_OBJECT
private:
	using Super = qf::qmlwidgets::framework::DialogWidget;

public:
	explicit StationsBackupMemoryWidget(QWidget *parent = nullptr);
	~StationsBackupMemoryWidget() Q_DECL_OVERRIDE;

private:
	Ui::StationsBackupMemoryWidget *ui;
	qf::core::model::SqlTableModel *m_tableModel;
};

