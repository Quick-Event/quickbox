#include "cardreadersettings.h"

CardReaderSettings::ReaderMode CardReaderSettings::readerModeEnum() const
{
	if(readerMode() == "EditOnPunch")
		return ReaderMode::EditOnPunch;
	return ReaderMode::Readout;
}
