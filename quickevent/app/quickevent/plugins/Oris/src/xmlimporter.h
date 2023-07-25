#ifndef XMLIMPORTER_H
#define XMLIMPORTER_H

#include <QObject>
#include <QXmlStreamReader>
#include <QSet>
#include <QMap>

class XmlImporter : public QObject
{
	Q_OBJECT
public:
	enum class XmlCreators
	{
		QuickEvent = 0,
		Oris,
		Eventor,
	};
	struct SPerson
	{
		QString nameGiven;
		QString nameFamily;

		QString className;
		QString classNameShort; // backup, if not defined className

		int iofId = -1;
		QString regCz;		// ORIS
		QString noteOris;	// ORIS
		int orisId = -1;	// ORIS

		int siNumber = 0;

		QString nationalityCode;	// Person
		QString nationalityName;	// Person
		QString clubCode;			// Organisation - Club
		QString clubName;			// Organisation - Club
		QString countryCode;		// Organisation - NationalFederation
		QString countryName;		// Organisation - NationalFederation
		int clubIdIof = -1;	// Eventor (Club or NationalFederation ID)
		QSet<int> enterRaces;
		// ...
		int bib = 0;
		int rank = 0;
		int start_order_pref = 0; // 0-none /  1- early / 2- middle / 3- late
		int leg = 0;
	};

	explicit XmlImporter(QObject *parent = nullptr);

	Q_INVOKABLE bool importXML30();
protected:
	bool readPersonNode(SPerson &s, QXmlStreamReader &reader, const XmlCreators creator);

	bool importEntries(QXmlStreamReader &reader, const XmlCreators creator);
	bool importStartlist(QXmlStreamReader &reader, const XmlCreators creator);
	bool importClasses(QXmlStreamReader &reader, const XmlCreators creator);
	bool importClubs(QXmlStreamReader &reader, const XmlCreators creator);
	bool importRegistration(QXmlStreamReader &reader, const XmlCreators creator);

	QString genFakeCzClubAbbr(QString country);
	QMap <QString,int> fakeCzClubMap;
};

#endif // XMLIMPORTER_H