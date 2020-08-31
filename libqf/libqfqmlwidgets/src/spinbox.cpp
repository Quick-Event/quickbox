#include "spinbox.h"
#include <QKeyEvent>
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

void SpinBox::keyPressEvent(QKeyEvent * event) {
	QString text;
	switch (event->nativeScanCode()) {
		case 10:
		case 11:
		case 12:
		case 13:
		case 14:
		case 15:
		case 16:
		case 17:
		case 18:
			text = QString::number(event->nativeScanCode() - 9);
			break;
		case 19:
			text = "0";
			break;
		default:
			text = event->text();
	}
	QKeyEvent * e = new QKeyEvent(event->type(), event->key(), event->modifiers(), text, event->isAutoRepeat(), event->count());
	Super::keyPressEvent(e);
}
