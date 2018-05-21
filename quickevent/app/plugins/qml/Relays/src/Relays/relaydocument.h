#ifndef RELAYS_RELAYDOCUMENT_H
#define RELAYS_RELAYDOCUMENT_H

#include "../relayspluginglobal.h"

#include <qf/core/model/sqldatadocument.h>

namespace Relays {

class RELAYSPLUGIN_DECL_EXPORT  RelayDocument : public qf::core::model::SqlDataDocument
{
	Q_OBJECT
private:
	typedef qf::core::model::SqlDataDocument Super;
public:
	 RelayDocument(QObject *parent = nullptr);

	bool isSaveSiidToRuns() const {return m_saveSiidToRuns;}
	void setSaveSiidToRuns(bool save_siid_to_runs) {m_saveSiidToRuns = save_siid_to_runs;}

	void setSiid(const QVariant &siid, bool save_siid_to_runs);
	void setSiid(const QVariant &siid);
	QVariant siid() const;
protected:
	bool saveData() Q_DECL_OVERRIDE;
	bool dropData() Q_DECL_OVERRIDE;
private:
	bool m_saveSiidToRuns = true;
};

}

#endif // RELAYS_RELAYDOCUMENT_H
