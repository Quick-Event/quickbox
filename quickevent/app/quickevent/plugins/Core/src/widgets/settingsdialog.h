#ifndef CORE_SETTINGSDIALOG_H
#define CORE_SETTINGSDIALOG_H

#include <QDialog>

class QButtonGroup;

namespace Core {

class SettingsPage;

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
	Q_OBJECT

public:
	explicit SettingsDialog(QWidget *parent = nullptr);
	~SettingsDialog();

	void addPage(SettingsPage *page);

	Q_SIGNAL void callForPages(Core::SettingsDialog *settings_dialog);
private slots:
	void on_buttonBox_rejected();
private:
	SettingsPage* page(int page_index);
private:
	Ui::SettingsDialog *ui;
	QButtonGroup *m_buttonGroup;
};


} // namespace Core
#endif // CORE_SETTINGSDIALOG_H
