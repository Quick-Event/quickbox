#ifndef COMPETITORS_COMPETITORDOCUMENT_H
#define COMPETITORS_COMPETITORDOCUMENT_H

#include <qf/core/model/sqldatadocument.h>

namespace Competitors {

class CompetitorDocument : public qf::core::model::SqlDataDocument
{
	Q_OBJECT
private:
	typedef qf::core::model::SqlDataDocument Super;
public:
	CompetitorDocument(QObject *parent = nullptr);

	Q_SIGNAL void competitorSaved(const QVariant &id,int mode);
	Q_INVOKABLE QString safeSave(bool save_siid_to_runs);
protected:
	bool saveData() Q_DECL_OVERRIDE;
	bool dropData() Q_DECL_OVERRIDE;
private:
	bool m_saveSiidToRuns = true;
};

}

#endif // COMPETITORS_COMPETITORDOCUMENT_H
