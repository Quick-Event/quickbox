#ifndef CARDREADER_CARDCHECKERCLASSICCPP_H
#define CARDREADER_CARDCHECKERCLASSICCPP_H

#include "cardchecker.h"

namespace CardReader {

class CardCheckerClassicCpp : public CardCheckerCpp
{
	Q_OBJECT
private:
	using Super = CardCheckerCpp;
public:
	CardCheckerClassicCpp(QObject *parent = nullptr);

	quickevent::core::si::CheckedCard checkCard(const quickevent::core::si::ReadCard &read_card) Q_DECL_OVERRIDE;
};

} // namespace CardReader

#endif // CARDCHECKERCLASSICCPP_H
