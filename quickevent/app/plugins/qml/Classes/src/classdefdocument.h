#ifndef CLASSDEFDOCUMENT_H
#define CLASSDEFDOCUMENT_H

#include <qf/core/model/sqldatadocument.h>

class ClassDefDocument : public qf::core::model::SqlDataDocument
{
	Q_OBJECT
private:
	typedef qf::core::model::SqlDataDocument Super;
public:
	ClassDefDocument(QObject *parent = nullptr);
};

#endif // CLASSDEFDOCUMENT_H
