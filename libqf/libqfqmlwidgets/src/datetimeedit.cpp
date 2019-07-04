#include "datetimeedit.h"

#include <qf/core/log.h>

namespace qf {
namespace qmlwidgets {

DateTimeEdit::DateTimeEdit(QWidget *parent)
	: Super(parent)
	, IDataWidget(this)
{
	connect(this, &QDateTimeEdit::dateTimeChanged, this, &DateTimeEdit::onDateTimeChanged);
}

QVariant DateTimeEdit::dataValue()
{
	return dateTime();
}

namespace {
class BlockerScope
{
public:
	explicit BlockerScope(bool &blocker_var) : m_blockerVar(blocker_var) {m_blockerVar = true;}
	~BlockerScope() {m_blockerVar = false;}
private:
	bool &m_blockerVar;
};
}

void DateTimeEdit::setDataValue(const QVariant &val)
{
	qfLogFuncFrame() << val;
	QDateTime new_datetime = val.toDateTime();
	if(checkSetDataValueFirstTime()) {
		setDateTime(new_datetime);
	}
	else {
		if(new_datetime != dateTime()) {
			BlockerScope bs(m_blockDateTimeChanged);
			setDateTime(new_datetime);
			saveDataValue();
			emit dataValueChanged(new_datetime);
		}
	}
}

void DateTimeEdit::onDateTimeChanged(const QDateTime &new_datetime)
{
	if(m_blockDateTimeChanged)
		return;
	qfLogFuncFrame() << new_datetime;
	saveDataValue();
	emit dataValueChanged(new_datetime);
}

} // namespace qmlwidgets
} // namespace qf
