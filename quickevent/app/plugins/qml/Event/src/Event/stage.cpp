#include "stage.h"
#include "stagedocument.h"

using namespace Event;

Stage::Stage(const QVariantMap &data)
	: Super(data)
{
}

Event::Stage::Stage(const Event::StageDocument *doc)
{
	for(auto s : {"id", "startTime", "startDate"}) {
		insert(s, doc->value(s));
	}
}
