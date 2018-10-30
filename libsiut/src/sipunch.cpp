#include "sipunch.h"

#include <qf/core/log.h>

namespace siut {

const SIPunch &SIPunch::sharedNull()
{
	static SIPunch n = SIPunch(SharedDummyHelper());
	return n;
}

SIPunch::SIPunch(SIPunch::SharedDummyHelper)
{
	d = new Data();
}

SIPunch::SIPunch()
{
	*this = sharedNull();
}

SIPunch::SIPunch(int code, int time)
{
	d = new Data;
	d->code = code;
	d->time = time;
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
	(...1023) (reserved)
	punching time PTH, PTL - 12h binary
	*/
	d = new Data;
	d->time = (uint16_t)getUnsigned(card_data, ix + 2, 2);
	d->code = (uint8_t)card_data[ix + 1];
	uint8_t pdt = (uint8_t)card_data[ix];
	d->pmFlag = pdt & 1;
	d->dayOfWeek = (DayOfWeek)((pdt & 0x0e) >> 1);
	d->weekCnt = ((pdt & 0x30) >> 4);
}

QVariantMap SIPunch::toVariantMap() const
{
	QVariantMap ret;
	ret[QStringLiteral("code")] = code();
	ret[QStringLiteral("time")] = time();
	ret[QStringLiteral("msec")] = msec();
	//ret[QStringLiteral("day")] = dayOfWeek();
	//ret[QStringLiteral("week")] = weekCnt();
	return ret;
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
