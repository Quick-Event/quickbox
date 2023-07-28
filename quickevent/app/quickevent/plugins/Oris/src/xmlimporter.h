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
		QString classNameShort;		// backup, if not defined className

		std::optional<int> iofId;
		QString regCz;				// ORIS
		QString noteOris;			// ORIS
		std::optional<int> orisId;	// ORIS

		int siNumber = 0;

		QString nationalityCode;		// Person
		QString nationalityName;		// Person
		QString clubCode;				// Organisation - Club
		QString clubName;				// Organisation - Club
		QString countryCode;			// Organisation - NationalFederation
		QString countryName;			// Organisation - NationalFederation
		std::optional<int> clubIdIof;	// Eventor (Club or NationalFederation ID)

		QSet<int> enterRaces;

		int legNumber = 0;
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
