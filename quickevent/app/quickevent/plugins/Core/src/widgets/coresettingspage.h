#ifndef CORESETTINGSPAGE_H
#define CORESETTINGSPAGE_H

#include "settingspage.h"

//#include <QWidget>

namespace Ui {
class CoreSettingsWidget;
}

class CoreSettingsPage : public Core::SettingsPage
{
	Q_OBJECT

	using Super = Core::SettingsPage;
public:
	explicit CoreSettingsPage(QWidget *parent = nullptr);
	~CoreSettingsPage();

private slots:
	void on_btSelectCustomReportsDirectory_clicked();
	void on_btExportReportDefinitions_clicked();
private:
	void load() override;
	void save() override;
private:
	Ui::CoreSettingsWidget *ui;
};

#endif // CORESETTINGSPAGE_H
