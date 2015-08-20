#include "checkbox.h"

namespace qf {
namespace qmlwidgets {

CheckBox::CheckBox(QWidget *parent)
	: Super(parent)
	, IDataWidget(this)
{
	connect(this, &QCheckBox::stateChanged, this, &CheckBox::onStateChanged);
}

QVariant CheckBox::dataValue()
{
	Qt::CheckState st = checkState();
	if(isTristate())
		return st;
	return (st == Qt::Checked);
}

void CheckBox::setDataValue(const QVariant &val)
{
	if(isTristate()) {
		int n = val.toInt();
		if(n != checkState()) {
			setCheckState(Qt::CheckState(n));
			saveDataValue();
			emit dataValueChanged(n);
		}
	}
	else {
		bool b = val.toBool();
		if(b != isChecked()) {
			setChecked(b);
			saveDataValue();
			emit dataValueChanged(b);
		}
	}
}

void CheckBox::onStateChanged(int st)
{
	saveDataValue();
	emit dataValueChanged(st);
}

} // namespace qmlwidgets
} // namespace qf

