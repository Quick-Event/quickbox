#include "timeedit.h"
#include "timems.h"

#include <qf/core/log.h>

using namespace quickevent::og;

TimeEdit::TimeEdit(QWidget *parent)
	: Super(parent)
{
	qfLogFuncFrame();
}

TimeEdit::~TimeEdit()
{

}

TimeMs TimeEdit::timeMs() const
{
	TimeMs t = TimeMs::fromString(text());
	return t;

}

void TimeEdit::setTimeMs(const TimeMs &t)
{
	qfLogFuncFrame();
	if(!(timeMs() == t)) {
		setText(t.toString());
		emit timeMsChanged();
	}
}

