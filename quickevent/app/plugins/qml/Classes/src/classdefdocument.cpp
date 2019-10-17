#include "classdefdocument.h"

ClassDefDocument::ClassDefDocument(QObject *parent)
	: Super(parent)
{
	qf::core::sql::QueryBuilder qb;
	qb.select2("classes", "name")
			.select2("classdefs", "*")
			.from("classdefs")
			.join("classdefs.classId", "classes.id")
			.where("classdefs.id={{ID}}");
	setQueryBuilder(qb);
}
