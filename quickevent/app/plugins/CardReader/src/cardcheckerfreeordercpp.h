#ifndef CARDREADER_CARDCHECKERFREEORDERCPP_H
#define CARDREADER_CARDCHECKERFREEORDERCPP_H

#include "cardchecker.h"

namespace CardReader {

class CardCheckerFreeOrderCpp : public CardCheckerCpp
{
	Q_OBJECT
private:
	using Super = CardCheckerCpp;
public:
	CardCheckerFreeOrderCpp(QObject *parent = nullptr);

	quickevent::core::si::CheckedCard checkCard(const quickevent::core::si::ReadCard &read_card) Q_DECL_OVERRIDE;
};

} // namespace CardReader

#endif // CARDCHECKERFREEORDERCPP_H
