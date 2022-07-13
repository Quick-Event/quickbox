#include "settingsdialog.h"
#include "ui_settingsdialog.h"

#include "settingspage.h"

#include <qf/core/log.h>

#include <QButtonGroup>
#include <QLabel>
#include <QPushButton>
#include <QTimer>
//#include <QDebug>

namespace Core {

SettingsDialog::SettingsDialog(QWidget *parent)
	: QDialog(parent)
	, ui(new Ui::SettingsDialog)
{
	ui->setupUi(this);
	m_buttonGroup = new QButtonGroup(this);
	connect(m_buttonGroup, &QButtonGroup::idToggled, this, [=](int page_index, bool checked) {
		qfDebug() << "id toggled:" << page_index << checked;
		if(checked) {
			ui->stackedWidget->setCurrentIndex(page_index);
			page(page_index)->load();
		}
		else {
			page(page_index)->save();
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
		page(page_index)->save();
	}
	accept();
}

SettingsPage *SettingsDialog::page(int page_index)
{
	auto *page = ui->stackedWidget->widget(page_index)->findChild<SettingsPage*>(QString(), Qt::FindDirectChildrenOnly);
	Q_ASSERT(page);
	return page;
}

void SettingsDialog::addPage(SettingsPage *page)
{
	auto caption = page->caption();
	Q_ASSERT(!caption.isEmpty());
	auto *layout = qobject_cast<QBoxLayout*>(ui->buttonsWidget->layout());
	Q_ASSERT(layout);
	int page_index = m_buttonGroup->buttons().count();
	auto *btn = new QPushButton(caption);
	// set widget minimum width to show all buttons, default behavior is to srt width of widget
	// according to width of first button added
	ui->buttonsWidget->setMinimumWidth(std::max(ui->buttonsWidget->minimumWidth(), btn->sizeHint().width() + layout->margin() * 5));
	btn->setCheckable(true);
	layout->insertWidget(page_index, btn);
	m_buttonGroup->addButton(btn, page_index);
	auto *frame = new QFrame();
	frame->setFrameStyle(QFrame::Box);
	auto *label = new QLabel("  " + caption + ' ' + tr("settings"));
	label->setObjectName("CaptionFrame"); // important for CSS
	auto *ly = new QVBoxLayout(frame);
	ly->setMargin(0);
	ly->addWidget(label);
	ly->addWidget(page);
	ui->stackedWidget->addWidget(frame);
	adjustSize();
	if(page_index == 0) {
		btn->click();
	}
}

} // namespace Core
