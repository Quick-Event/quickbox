#ifndef RUNS_NSTAGESREPORTOPTIONSDIALOG_H
#define RUNS_NSTAGESREPORTOPTIONSDIALOG_H

#include <QDialog>

namespace Runs {

namespace Ui {
class NStagesReportOptionsDialog;
}

class NStagesReportOptionsDialog : public QDialog
{
	Q_OBJECT

	Q_PROPERTY(int stagesCount READ stagesCount WRITE setStagesCount)
	Q_PROPERTY(int maxPlacesCount READ maxPlacesCount WRITE setMaxPlacesCount)
	Q_PROPERTY(bool excludeDisqualified READ isExcludeDisqualified WRITE setExcludeDisqualified)
public:
	explicit NStagesReportOptionsDialog(QWidget *parent = 0);
	~NStagesReportOptionsDialog() Q_DECL_OVERRIDE;
private:
	int stagesCount() const;
	void setStagesCount(int n);

	int maxPlacesCount() const;
	void setMaxPlacesCount(int n);

	bool isExcludeDisqualified() const;
	void setExcludeDisqualified(bool b);
private:
	Ui::NStagesReportOptionsDialog *ui;
};


} // namespace Runs
#endif // RUNS_NSTAGESREPORTOPTIONSDIALOG_H
