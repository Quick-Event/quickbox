#ifndef COMPETITORS_COMPETITORDOCUMENT_H
#define COMPETITORS_COMPETITORDOCUMENT_H

#include <qf/core/model/sqldatadocument.h>

#include <QVector>

namespace Competitors {

class CompetitorDocument : public qf::core::model::SqlDataDocument
{
	Q_OBJECT
private:
	typedef qf::core::model::SqlDataDocument Super;
public:
	CompetitorDocument(QObject *parent = nullptr);

	//bool isSaveSiidToRuns() const {return m_saveSiidToRuns;}
	void setEmitDbEventsOnSave(bool b) {m_isEmitDbEventsOnSave = b;}

	//void setSiid(const QVariant &siid, bool save_siid_to_runs);
	void setSiid(const QVariant &siid);
	QVariant siid() const;
	const QVector<int>& lastInsertedRunsIds() const {return m_lastInsertedRunsIds;}
protected:
	bool saveData() Q_DECL_OVERRIDE;
	bool dropData() Q_DECL_OVERRIDE;
private:
	bool m_isEmitDbEventsOnSave = true;
	QVector<int> m_lastInsertedRunsIds;
};

}

#endif // COMPETITORS_COMPETITORDOCUMENT_H
