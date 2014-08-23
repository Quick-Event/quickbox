#include "dialogwidget.h"

using namespace qf::qmlwidgets::framework;

DialogWidget::DialogWidget(QWidget *parent) :
	Super(parent), IPersistentSettings(this)
{
}

DialogWidget::~DialogWidget()
{
}
