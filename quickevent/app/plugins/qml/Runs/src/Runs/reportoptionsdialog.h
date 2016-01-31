#ifndef RUNS_REPORTOPTIONSDIALOG_H
#define RUNS_REPORTOPTIONSDIALOG_H

#include <QDialog>
#include <QVariantMap>

namespace Runs {

namespace Ui {
class ReportOptionsDialog;
}

class ReportOptionsDialog : public QDialog
{
	Q_OBJECT

public:
	explicit ReportOptionsDialog(QWidget *parent = 0);
	~ReportOptionsDialog();

	//Q_INVOKABLE QVariantMap optionsToMap() const;
	Q_INVOKABLE bool isEveryClassOnNewPage() const;
	Q_INVOKABLE QString sqlWhereExpression() const;
private:
	Ui::ReportOptionsDialog *ui;
};


} // namespace Runs
#endif // RUNS_REPORTOPTIONSDIALOG_H
