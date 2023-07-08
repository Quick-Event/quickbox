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
	explicit TxtImporter(QObject *parent = nullptr);

	Q_INVOKABLE void importCompetitorsCSOS();
	Q_INVOKABLE void importCompetitorsCSV();
	Q_INVOKABLE void importRankingCsv();
	Q_INVOKABLE void importRunsCzeCSV();
	Q_INVOKABLE void importRunsIofCSV();
protected:
	void importParsedCsv(const QList<QVariantList> &csv);
};

#endif // TXTIMPORTER_H
