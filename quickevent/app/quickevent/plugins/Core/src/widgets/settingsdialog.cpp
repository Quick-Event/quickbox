#include "settingsdialog.h"
#include "ui_settingsdialog.h"

#include "settingspage.h"

#include <qf/core/log.h>

#include <QButtonGroup>
#include <QPushButton>
#include <QTimer>

namespace Core {

SettingsDialog::SettingsDialog(QWidget *parent)
	: QDialog(parent)
	, ui(new Ui::SettingsDialog)
{
	ui->setupUi(this);
	m_buttonGroup = new QButtonGroup(this);
	connect(m_buttonGroup, &QButtonGroup::idToggled, this, [=](int page_index, bool checked) {
		qfDebug() << "id toggled:" << page_index << checked;
		auto *page = qobject_cast<SettingsPage*>(ui->stackedWidget->widget(page_index));
		Q_ASSERT(page);
		if(checked) {
			ui->stackedWidget->setCurrentIndex(page_index);
			page->load();
		}
		else {
			page->save();
		}
	});
}

SettingsDialog::~SettingsDialog()
{
	delete ui;
}

void SettingsDialog::on_buttonBox_rejected()
{
	if(int page_index = m_buttonGroup->checkedId(); page_index >= 0) {
		auto *page = qobject_cast<SettingsPage*>(ui->stackedWidget->widget(page_index));
		Q_ASSERT(page);
		page->save();
	}
	accept();
}

void SettingsDialog::addPage(SettingsPage *page)
{
	auto caption = page->caption();
	Q_ASSERT(!caption.isEmpty());
	auto *layout = qobject_cast<QBoxLayout*>(ui->buttonsWidget->layout());
	Q_ASSERT(layout);
	int page_index = m_buttonGroup->buttons().count();
	auto *btn = new QPushButton(caption);
	//btn->setAutoExclusive(true);
	btn->setCheckable(true);
	layout->insertWidget(page_index, btn);
	m_buttonGroup->addButton(btn, page_index);
	ui->stackedWidget->addWidget(page);
	adjustSize();
	if(page_index == 0) {
		btn->click();
	}
}

} // namespace Core
