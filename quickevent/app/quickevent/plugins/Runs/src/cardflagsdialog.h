#pragma once

#include <QDialog>

namespace quickevent::core::og { class SqlTableModel; }

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

	void load(quickevent::core::og::SqlTableModel *model, int row);
	void save();
private:
	void updateStatus();
private:
	Ui::CardFlagsDialog *ui;
	quickevent::core::og::SqlTableModel *m_model;
	int m_row;
};


} // namespace Runs
