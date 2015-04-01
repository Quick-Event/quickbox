#ifndef COMPETITORS_COMPETITORDOCUMENT_H
#define COMPETITORS_COMPETITORDOCUMENT_H

#include <qf/core/model/sqldatadocument.h>

class CompetitorDocument : public qf::core::model::SqlDataDocument
{
	Q_OBJECT
private:
	typedef qf::core::model::SqlDataDocument Super;
public:
	CompetitorDocument(QObject *parent = nullptr);
protected:
	bool saveData() Q_DECL_OVERRIDE;
	bool dropData() Q_DECL_OVERRIDE;
};

#endif // COMPETITORS_COMPETITORDOCUMENT_H
