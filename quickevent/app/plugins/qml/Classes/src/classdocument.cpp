#include "classdocument.h"

#include <qf/core/sql/querybuilder.h>

ClassDocument::ClassDocument(QObject *parent)
	: Super(parent)
{
	qf::core::sql::QueryBuilder qb;
	qb.select2("classes", "*")
			.from("classes")
			.where("classes.id={{ID}}");
	setQueryBuilder(qb);
}

