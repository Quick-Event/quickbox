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
	enum class BreakType : int {None = 0, Column, Page };
public:
	explicit ReportOptionsDialog(QWidget *parent = 0);
	~ReportOptionsDialog();

	BreakType breakType() const;
	//Q_INVOKABLE QVariantMap optionsToMap() const;
	Q_INVOKABLE bool isBreakAfterEachClass() const {return breakType() != BreakType::None;}
	Q_INVOKABLE bool isColumnBreak() const {return breakType() == BreakType::Column;}
	Q_INVOKABLE QString sqlWhereExpression() const;
private:
	Ui::ReportOptionsDialog *ui;
};


} // namespace Runs
#endif // RUNS_REPORTOPTIONSDIALOG_H
