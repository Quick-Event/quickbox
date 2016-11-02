#ifndef COMPETITORS_COMPETITORDOCUMENT_H
#define COMPETITORS_COMPETITORDOCUMENT_H

#include "../competitorspluginglobal.h"

#include <qf/core/model/sqldatadocument.h>

namespace Competitors {

class COMPETITORSPLUGIN_DECL_EXPORT CompetitorDocument : public qf::core::model::SqlDataDocument
{
	Q_OBJECT
private:
	typedef qf::core::model::SqlDataDocument Super;
public:
	CompetitorDocument(QObject *parent = nullptr);

	//bool isSaveSiidToRuns() const;
	//void setSaveSiidToRuns(bool save_siid_to_runs);

	void setSiid(int siid, bool is_unique);
	int uniqueSiid() const {return m_uniqueSiid;}
	//bool isSiidUnique() const {return uniqueSiid() > 0;}
protected:
	bool saveData() Q_DECL_OVERRIDE;
	bool dropData() Q_DECL_OVERRIDE;
private:
	//bool m_saveSiidToRuns = true;
	int m_uniqueSiid = 0; //< if filled, it is saved to runs
};

}

#endif // COMPETITORS_COMPETITORDOCUMENT_H
