#ifndef RELAYWIDGET_H
#define RELAYWIDGET_H

#include <qf/qmlwidgets/framework/datadialogwidget.h>

namespace quickevent {
namespace og {
class SqlTableModel;
} }

namespace Ui {
class  RelayWidget;
}

class  RelayWidget : public qf::qmlwidgets::framework::DataDialogWidget
{
	Q_OBJECT
private:
	typedef qf::qmlwidgets::framework::DataDialogWidget Super;
public:
	explicit  RelayWidget(QWidget *parent = 0);
	~ RelayWidget() Q_DECL_OVERRIDE;

	bool load(const QVariant &id = QVariant(), int mode = qf::core::model::DataDocument::ModeEdit) Q_DECL_OVERRIDE;
	void loadFromRegistrations(int siid);
	//Q_SIGNAL void editStartListRequest(int stage_id, int class_id, int competitor_id);

private slots:
	void onRegistrationSelected(const QVariantMap &values);
private:
	Q_SLOT bool loadRunsTable();
	Q_SLOT bool saveRunsTable();
	//void onRunsTableCustomContextMenuRequest(const QPoint &pos);
	bool saveData() Q_DECL_OVERRIDE;

	QVector<int> juniorAges();
	QVector<int> veteranAges();
	QString classNameFromRegistration(const QString &registration);

	void showRunsTable(int stage_id);
private:
	Ui:: RelayWidget *ui;
	quickevent::og::SqlTableModel *m_runsModel;
};

#endif // RELAYWIDGET_H
