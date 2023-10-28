#ifndef RUNS_RUNFLAGSDIALOG_H
#define RUNS_RUNFLAGSDIALOG_H

#include <QDialog>

namespace qf {
namespace core {
namespace model {

class SqlTableModel;

}}}

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

	void load(qf::core::model::SqlTableModel *model, int row);
	void save();
private:
	void updateStatus();
	bool isDisqualified() const;
private:
	Ui::RunFlagsDialog *ui;
	qf::core::model::SqlTableModel *m_model;
	int m_row;
	bool m_isDisqualified;
};


} // namespace Runs
#endif // RUNS_RUNFLAGSDIALOG_H
