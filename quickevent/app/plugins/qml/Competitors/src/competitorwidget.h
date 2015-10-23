#ifndef COMPETITORWIDGET_H
#define COMPETITORWIDGET_H

#include <qf/qmlwidgets/framework/datadialogwidget.h>

namespace quickevent {
namespace og {
class SqlTableModel;
} }

namespace Ui {
class CompetitorWidget;
}

class CompetitorWidget : public qf::qmlwidgets::framework::DataDialogWidget
{
	Q_OBJECT
private:
	typedef qf::qmlwidgets::framework::DataDialogWidget Super;
public:
	explicit CompetitorWidget(QWidget *parent = 0);
	~CompetitorWidget() Q_DECL_OVERRIDE;

	bool load(const QVariant &id = QVariant(), int mode = qf::core::model::DataDocument::ModeEdit) Q_DECL_OVERRIDE;

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
	Ui::CompetitorWidget *ui;
	quickevent::og::SqlTableModel *m_runsModel;
};

#endif // COMPETITORWIDGET_H
