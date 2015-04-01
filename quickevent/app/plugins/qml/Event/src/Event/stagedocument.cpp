#include "stagedocument.h"

#include <qf/core/sql/querybuilder.h>

using namespace Event;

StageDocument::StageDocument(QObject *parent)
	: Super(parent)
{
	qf::core::sql::QueryBuilder qb;
	qb.select2("stages", "*")
			.from("stages")
			.where("stages.id={{ID}}");
	setQueryBuilder(qb);
}

