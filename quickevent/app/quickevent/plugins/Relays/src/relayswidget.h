#pragma once

#include <QFrame>

#include <quickevent/gui/partwidget.h>

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
	explicit RelaysWidget(QWidget *parent = nullptr);
	~RelaysWidget() Q_DECL_OVERRIDE;

	void settleDownInPartWidget(quickevent::gui::PartWidget *part_widget);
private:
	Q_SLOT void lazyInit();
	Q_SLOT void reset();
	Q_SLOT void reload();

	void editRelay(const QVariant &id, int mode);
	void editRelays(int mode);

	//void onDbEventNotify(const QString &domain, int connection_id, const QVariant &data);

	QVariant startListByClubsTableData();

	void relays_assignNumbers();
	void relays_importBibs();

	void print_start_list_classes();
	void print_start_list_clubs();

	void print_results_nlegs();
	void print_results_overal();
	void print_results_overal_condensed();

	void export_results_iofxml3();
	void export_start_list_iofxml3();

	//void printResults(const QString &settings_id, const QVariantMap &default_options);
private:
	Ui::RelaysWidget *ui;
	qf::core::model::SqlTableModel *m_tblModel;
	qf::qmlwidgets::ForeignKeyComboBox *m_cbxClasses = nullptr;
	void save_xml_file(QString str, QString fn);
};

