#include "emmaclientwidget.h"
#include "ui_emmaclientwidget.h"
#include "emmaclient.h"

#include <qf/qmlwidgets/dialogs/messagebox.h>

#include <qf/core/assert.h>

#include <QFileDialog>
#include <QSettings>

namespace services {

EmmaClientWidget::EmmaClientWidget(QWidget *parent)
	: Super(parent)
	, ui(new Ui::EmmaClientWidget)
{
	setPersistentSettingsId("EmmaClientWidget");
	ui->setupUi(this);

	EmmaClient *svc = service();
	if(svc) {
		ui->edFileName->setText(svc->fileName());
	}

	connect(ui->btChooseFile, &QPushButton::clicked, this, &EmmaClientWidget::onBtChooseFileClicked);
}

EmmaClientWidget::~EmmaClientWidget()
{
	delete ui;
}

void EmmaClientWidget::onBtChooseFileClicked()
{
	QString fn = QFileDialog::getOpenFileName(this, tr("Open File"), ui->edFileName->text(), tr("Text Files (*.txt)"));
	if(!fn.isEmpty())
		ui->edFileName->setText(fn);
}

bool EmmaClientWidget::acceptDialogDone(int result)
{
	if(result == QDialog::Accepted) {
		QString fn = ui->edFileName->text().trimmed();
		if(!fn.isEmpty()) {
			QFile file(fn);
			if(!file.open(QIODevice::WriteOnly | QIODevice::Append)) {
				qf::qmlwidgets::dialogs::MessageBox::showError(this, tr("File '%1' cannot be open for writing.").arg(fn));
				return false;
			}
		}
		EmmaClient *svc = service();
		if(svc) {
			svc->setFileName(fn);
			svc->loadSettings();
		}
	}
	return true;
}

EmmaClient *EmmaClientWidget::service()
{
	EmmaClient *svc = qobject_cast<EmmaClient*>(Service::serviceByName(EmmaClient::serviceName()));
	QF_ASSERT(svc, EmmaClient::serviceName() + " doesn't exist", return nullptr);
	return svc;
}

}
