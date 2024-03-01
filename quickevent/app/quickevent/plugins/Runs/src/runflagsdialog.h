#pragma once

#include <QDialog>

namespace quickevent::core::og { class SqlTableModel; }

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

	void load(quickevent::core::og::SqlTableModel *model, int row);
	void save();
private:
	void updateStatus();
	bool isDisqualified() const;
private:
	Ui::RunFlagsDialog *ui;
	quickevent::core::og::SqlTableModel *m_model;
	int m_row;
	bool m_isDisqualified;
};

} // namespace Runs
