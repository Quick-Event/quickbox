#ifndef TXTIMPORTER_H
#define TXTIMPORTER_H

#include <QObject>

class TxtImporter : public QObject
{
	Q_OBJECT
public:
	enum TxtImportColumns {
		ColRegistration = 0,
		ColClassName,
		ColSiId,
		ColLastName,
		ColFirstName,
		ColLicence,
		ColNote,
		ColCount,
	};

public:
	explicit TxtImporter(QObject *parent = 0);

	Q_INVOKABLE void importCompetitorsCSOS();
	Q_INVOKABLE void importRankingCsv();
protected:
	void importParsedCsv(const QList<QVariantList> &csv);
};

#endif // TXTIMPORTER_H
