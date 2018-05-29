#pragma once

#include <QFrame>

class QCheckBox;

namespace Ui {
class RelaysWidget;
}
namespace qf {
namespace core {
namespace model {
class SqlTableModel;
}
}
namespace qmlwidgets {
class ForeignKeyComboBox;
}
}

class ThisPartWidget;

class RelaysWidget : public QFrame
{
	Q_OBJECT
private:
	typedef QFrame Super;
public:
	explicit RelaysWidget(QWidget *parent = 0);
	~RelaysWidget() Q_DECL_OVERRIDE;

	void settleDownInPartWidget(ThisPartWidget *part_widget);
private:
	Q_SLOT void lazyInit();
	Q_SLOT void reset();
	Q_SLOT void reload();

	void editRelay(const QVariant &id, int mode);
	void editRelays(int mode);

	//void onDbEventNotify(const QString &domain, int connection_id, const QVariant &data);

	QVariant startListTableData(const QString &class_filter);

	void print_start_list_classes();
private:
	Ui::RelaysWidget *ui;
	qf::core::model::SqlTableModel *m_tblModel;
	qf::qmlwidgets::ForeignKeyComboBox *m_cbxClasses = nullptr;
};

