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
protected:
	bool saveData() Q_DECL_OVERRIDE;
	bool dropData() Q_DECL_OVERRIDE;
};

#endif // CLASSDOCUMENT_H
