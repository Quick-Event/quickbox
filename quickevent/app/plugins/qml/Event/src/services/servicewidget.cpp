#include "servicewidget.h"
#include "ui_servicewidget.h"

namespace services {

ServiceWidget::ServiceWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::ServiceWidget)
{
	ui->setupUi(this);

	connect(ui->btPlay, &QPushButton::clicked, this, &ServiceWidget::onBtPlayClicked);
	connect(ui->btShowDetail, &QPushButton::clicked, this, &ServiceWidget::showDetail);
}

ServiceWidget::~ServiceWidget()
{
	delete ui;
}

void ServiceWidget::setStatus(Service::Status st)
{
	m_isRunning = st == Service::Status::Running;
	static QIcon ico_play(":/qf/qmlwidgets/images/flat/media-play");
	static QIcon ico_stop(":/qf/qmlwidgets/images/flat/media-stop");
	ui->btPlay->setIcon(m_isRunning? ico_stop: ico_play);
	switch (st) {
	case Service::Status::Running:
		ui->lblStatus->setPixmap(QPixmap(":/qf/core/images/light-green"));
		break;
	case Service::Status::Stopped:
		ui->lblStatus->setPixmap(QPixmap(":/qf/core/images/light-red"));
		break;
	default:
		ui->lblStatus->setPixmap(QPixmap(":/qf/core/images/light-yellow"));
		break;
	}
}

void ServiceWidget::setServiceName(const QString &n)
{
	ui->lblServiceName->setText(n);
}

QString ServiceWidget::serviceName() const
{
	return ui->lblServiceName->text();
}

void ServiceWidget::setMessage(const QString &m)
{
	ui->lblServiceMessage->setText(m);
}

void ServiceWidget::onBtPlayClicked()
{
	emit setRunningRequest(!m_isRunning);
}

void ServiceWidget::showDetail()
{
	Service *svc = Service::serviceByName(serviceName());
	if(svc) {
		svc->showDetail(this);
	}
}

} // namespace services
