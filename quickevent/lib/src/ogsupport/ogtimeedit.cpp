#include "ogtimeedit.h"
#include "ogtimems.h"

#include <qf/core/log.h>

OGTimeEdit::OGTimeEdit(QWidget *parent)
	: Super(parent)
{
	qfLogFuncFrame();
}

OGTimeEdit::~OGTimeEdit()
{

}

OGTimeMs OGTimeEdit::ogTimeMs() const
{
	OGTimeMs t = OGTimeMs::fromString(text());
	return t;

}

void OGTimeEdit::setOGTimeMs(const OGTimeMs &t)
{
	qfLogFuncFrame();
	if(ogTimeMs().msec() != t.msec()) {
		setText(t.toString());
		emit ogTimeMsChanged();
	}
}

