#ifndef RELAYWIDGET_H
#define RELAYWIDGET_H

#include <qf/qmlwidgets/framework/datadialogwidget.h>

namespace quickevent { namespace og { class SqlTableModel; } }

namespace Ui { class  RelayWidget; }

class  RelayWidget : public qf::qmlwidgets::framework::DataDialogWidget
{
	Q_OBJECT
private:
	typedef qf::qmlwidgets::framework::DataDialogWidget Super;
public:
	explicit  RelayWidget(QWidget *parent = 0);
	~RelayWidget() Q_DECL_OVERRIDE;

	bool load(const QVariant &id = QVariant(), int mode = qf::core::model::DataDocument::ModeEdit) Q_DECL_OVERRIDE;
private:
	Q_SLOT bool loadLegsTable();
	//Q_SLOT bool saveLegsTable();
	//void onRunsTableCustomContextMenuRequest(const QPoint &pos);
	bool saveData() Q_DECL_OVERRIDE;

	void addLeg();
	void removeLeg();
	void moveLegUp();
	void moveLegDown();
private:
	Ui:: RelayWidget *ui;
	quickevent::og::SqlTableModel *m_legsModel;
};

#endif // RELAYWIDGET_H
