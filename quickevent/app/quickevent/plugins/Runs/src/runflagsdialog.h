#ifndef RUNS_RUNFLAGSDIALOG_H
#define RUNS_RUNFLAGSDIALOG_H

#include <QDialog>

class RunsTableModel;

namespace Runs {

namespace Ui {
class RunFlagsDialog;
}

class RunFlagsDialog : public QDialog
{
	Q_OBJECT
public:
	explicit RunFlagsDialog(QWidget *parent = nullptr);
	~RunFlagsDialog() override;

	void load(RunsTableModel *model, int row);
	void save();
private:
	void updateState();
private:
	Ui::RunFlagsDialog *ui;
	RunsTableModel *m_model;
	int m_row;
};


} // namespace Runs
#endif // RUNS_RUNFLAGSDIALOG_H
