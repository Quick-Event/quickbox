#ifndef CLASSDOCUMENT_H
#define CLASSDOCUMENT_H

#include <qf/core/model/sqldatadocument.h>

class ClassDocument : public qf::core::model::SqlDataDocument
{
	Q_OBJECT
private:
	typedef qf::core::model::SqlDataDocument Super;
public:
	ClassDocument(QObject *parent = nullptr);
};

#endif // CLASSDOCUMENT_H
