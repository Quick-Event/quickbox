#include "timeedit.h"

#include <qf/core/log.h>

using namespace qf::qmlwidgets;

TimeEdit::TimeEdit(QWidget *parent)
	: Super(parent)
	, IDataWidget(this)
{
	connect(this, &QTimeEdit::timeChanged, this, &TimeEdit::onTimeChanged);
}

QVariant TimeEdit::dataValue()
{
	return time();
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

void TimeEdit::setDataValue(const QVariant &val)
{
	qfLogFuncFrame() << val;
	QTime new_time = val.toTime();
	if(checkSetDataValueFirstTime()) {
		setTime(new_time);
	}
	else {
		if(new_time != time()) {
			BlockerScope bs(m_blockTimeChanged);
			setTime(new_time);
			saveDataValue();
			emit dataValueChanged(new_time);
		}
	}
}

void TimeEdit::onTimeChanged(const QTime &new_time)
{
	if(m_blockTimeChanged)
		return;
	qfLogFuncFrame() << new_time;
	saveDataValue();
	emit dataValueChanged(new_time);
}
