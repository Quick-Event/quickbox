#include "logwidget.h"
#include "ui_logwidget.h"

namespace qf {
namespace qmlwiggets {
namespace framework {

LogWidget::LogWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::LogWidget)
{
	ui->setupUi(this);
}

LogWidget::~LogWidget()
{
	delete ui;
}

} // namespace framework
} // namespace qmlwiggets
} // namespace qf
