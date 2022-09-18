#ifndef REPORTSSETTINGSPAGE_H
#define REPORTSSETTINGSPAGE_H

#include "settingspage.h"

namespace Core {

namespace Ui {
class ReportsSettingsPage;
}

class ReportsSettingsPage : public Core::SettingsPage
{
	Q_OBJECT

	using Super = Core::SettingsPage;
public:
	explicit ReportsSettingsPage(QWidget *parent = nullptr);
	~ReportsSettingsPage();

	QString customReportsDirectory() const;
private slots:
	void on_btSelectCustomReportsDirectory_clicked();
	void on_btCreateCustomReportsDirectory_clicked();

private:
	void load() override;
	void save() override;

	void setCustomReportsDirectory(const QString dir);
private:
	Ui::ReportsSettingsPage *ui;
	//QString m_exportReportDefinitionsDir;
};

}
#endif // REPORTSSETTINGSPAGE_H
