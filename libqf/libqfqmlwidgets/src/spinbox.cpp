#include "spinbox.h"

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
