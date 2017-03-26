#ifndef CARDREADER_CARDCHECKERCLASSICCPP_H
#define CARDREADER_CARDCHECKERCLASSICCPP_H

#include "cardchecker.h"

namespace CardReader {

class CardCheckerClassicCpp : public CardChecker, public CppCardChecker
{
	Q_OBJECT
private:
	using Super = CardChecker;
public:
	CardCheckerClassicCpp(QObject *parent = 0);

	CheckedCard checkCard(const ReadCard &read_card) Q_DECL_OVERRIDE;
};

} // namespace CardReader

#endif // CARDCHECKERCLASSICCPP_H
