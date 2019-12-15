#ifndef COMPETITORS_COMPETITORDOCUMENT_H
#define COMPETITORS_COMPETITORDOCUMENT_H

#include "competitorspluginglobal.h"

#include <qf/core/model/sqldatadocument.h>

#include <QVector>

namespace Competitors {

class COMPETITORSPLUGIN_DECL_EXPORT CompetitorDocument : public qf::core::model::SqlDataDocument
{
	Q_OBJECT
private:
	typedef qf::core::model::SqlDataDocument Super;
public:
	CompetitorDocument(QObject *parent = nullptr);

	//bool isSaveSiidToRuns() const {return m_saveSiidToRuns;}
	//void setSaveSiidToRuns(bool save_siid_to_runs) {m_saveSiidToRuns = save_siid_to_runs;}

	//void setSiid(const QVariant &siid, bool save_siid_to_runs);
	void setSiid(const QVariant &siid);
	QVariant siid() const;
	const QVector<int>& lastInsertedRunsIds() const {return m_lastInsertedRunsIds;}
protected:
	bool saveData() Q_DECL_OVERRIDE;
	bool dropData() Q_DECL_OVERRIDE;
private:
	//bool m_saveSiidToRuns = true;
	QVector<int> m_lastInsertedRunsIds;
};

}

#endif // COMPETITORS_COMPETITORDOCUMENT_H
