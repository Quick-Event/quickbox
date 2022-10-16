#ifndef RUNS_CARDFLAGSDIALOG_H
#define RUNS_CARDFLAGSDIALOG_H

#include <QDialog>

class RunsTableModel;

namespace Runs {

namespace Ui {
class CardFlagsDialog;
}

class CardFlagsDialog : public QDialog
{
	Q_OBJECT

public:
	explicit CardFlagsDialog(QWidget *parent = nullptr);
	~CardFlagsDialog() override;

	void load(RunsTableModel *model, int row);
	void save();
private:
	void updateStatus();
private:
	Ui::CardFlagsDialog *ui;
	RunsTableModel *m_model;
	int m_row;
};


} // namespace Runs
#endif // RUNS_CARDFLAGSDIALOG_H
