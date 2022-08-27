#include "sipunch.h"

#include <qf/core/log.h>

namespace siut {

SIPunch::SIPunch()
{
}

SIPunch::SIPunch(int code, int time)
{
	setCode(code);
	setTime(time);
}

SIPunch::SIPunch(const QByteArray &card_data, int ix)
{
	/*
	record structure: PTD - CN - PTH - PTL
	CN - control station code number, 0...255 or subsecond value1
	PTD - day of week / halfday
	bit 0 - am/pm
	bit 3...1 - day of week, 000 = Sunday, 110 = Saturday
	bit 5...4 - week counter 0…3, relative
	bit 7...6 - control station code number high
	(...511)
	punching time PTH, PTL - 12h binary
	*/
	setTime((uint16_t)getUnsigned(card_data, ix + 2, 2));
	uint8_t pdt = (uint8_t)card_data[ix];
	uint16_t code_complete = ((pdt & 0x60) >> 6) << 8;
	code_complete += (uint8_t)card_data[ix + 1];
	setCode(code_complete);
	setPmFlag(pdt & 1);
	setDayOfWeek((pdt & 0x0e) >> 1);
	setWeekCnt((pdt & 0x30) >> 4);
}

unsigned SIPunch::getUnsigned(const QByteArray &ba, int ix, int byte_cnt)
{
	unsigned ret = 0;
	if(ix + byte_cnt <= ba.size()) {
		for (int i = 0; i < byte_cnt; ++i) {
			ret <<= 8;
			ret += static_cast<uint8_t>(ba[ix + i]);
		}
	}
	else {
		qfError() << "array too short";
	}
	return ret;
}

} // namespace siut
