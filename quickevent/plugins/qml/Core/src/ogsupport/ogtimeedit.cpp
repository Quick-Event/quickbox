#include "ogtimeedit.h"
#include "ogtimems.h"

OGTimeEdit::OGTimeEdit(QWidget *parent)
	: Super(parent)
{

}

OGTimeEdit::~OGTimeEdit()
{

}

int OGTimeEdit::obTimeMs() const
{
	OGTimeMs t = OGTimeMs::fromString(text());
	return t.msec();

}

void OGTimeEdit::setObTimeMs(int msec)
{
	if(obTimeMs() != msec) {
		OGTimeMs t(msec);
		setText(t.toString());
		emit obTimeMsChanged(msec);
	}
}

