#include "spinbox.h"
#include <QKeyEvent>
#include <QCoreApplication>
using namespace qf::qmlwidgets;

SpinBox::SpinBox(QWidget *parent)
	: Super(parent)
	, IDataWidget(this)
{
	connect(this, SIGNAL(valueChanged(int)), this, SLOT(onValueChanged(int)));
}

QVariant SpinBox::dataValue()
{
	return value();
}

void SpinBox::setDataValue(const QVariant &val)
{
	int n = val.toInt();
	if(checkSetDataValueFirstTime()) {
		setValue(n);
	}
	else {
		if(n != value()) {
			setValue(n);
			saveDataValue();
			emit dataValueChanged(n);
		}
	}
}

void SpinBox::onValueChanged(int n)
{
	saveDataValue();
	emit dataValueChanged(n);
}

void SpinBox::keyPressEvent(QKeyEvent *event) {
	QString text;
	auto code = event->nativeScanCode();
#ifdef __unix__
	// Xorg has an offset of 8 due to historical reasons
	code = code - 8;
#endif
	switch (code) {
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:
		case 10:
			text = QString::number(code - 1);
			break;
		case 11:
			text = "0";
			break;
		default:
			Super::keyPressEvent(event);
			return;
	}
	QKeyEvent *fake_event = new QKeyEvent(event->type(), event->key(), event->modifiers(), text, event->isAutoRepeat(), event->count());
	QCoreApplication::postEvent(this, fake_event);
}
