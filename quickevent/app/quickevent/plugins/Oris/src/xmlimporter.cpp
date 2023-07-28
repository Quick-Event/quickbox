#include "xmlimporter.h"

#include <qf/qmlwidgets/framework/mainwindow.h>
#include <qf/qmlwidgets/dialogs/filedialog.h>
#include <qf/qmlwidgets/dialogs/messagebox.h>

#include <plugins/Event/src/eventplugin.h>
#include <plugins/Classes/src/classesplugin.h>
#include <plugins/Classes/src/classdocument.h>
#include <plugins/Competitors/src/competitordocument.h>

#include <qf/core/log.h>
#include <qf/core/sql/query.h>
#include <qf/core/sql/transaction.h>
#include <QInputDialog>

namespace qfw = qf::qmlwidgets;
namespace qfd = qf::qmlwidgets::dialogs;
using qf::qmlwidgets::framework::getPlugin;
using Event::EventPlugin;

XmlImporter::XmlImporter(QObject *parent)
	: QObject(parent)
{
}

bool XmlImporter::readPersonNode (SPerson &s, QXmlStreamReader &reader, [[maybe_unused]] const XmlCreators creator)
{
	bool result = false;
	while(reader.readNextStartElement()) {
		if (reader.name() == "Person") {
			while(reader.readNextStartElement()) {
				if (reader.name() == "Id") {
					if (reader.attributes().hasAttribute("type") && reader.attributes().value("type").toString() == "IOF")
						s.iofId = reader.readElementText().toInt();
					else if (reader.attributes().hasAttribute("type") && reader.attributes().value("type").toString() == "CZE")
						s.regCz = reader.readElementText();
					else if (reader.attributes().hasAttribute("type") && reader.attributes().value("type").toString() == "ORIS")
						s.orisId = reader.readElementText().toInt();
					else
						reader.skipCurrentElement();
					result = (s.iofId.has_value() || !s.regCz.isEmpty());
				}
				else if (reader.name() == "Nationality") {
					if (reader.attributes().hasAttribute("code"))
						s.nationalityCode = reader.attributes().value("code").toString();
					s.nationalityName = reader.readElementText();
				}
				else if (reader.name() == "Name") {
					while(reader.readNextStartElement()) {
						if (reader.name() == "Family")
							s.nameFamily = reader.readElementText();
						else if (reader.name() == "Given")
							s.nameGiven = reader.readElementText();
						else
							reader.skipCurrentElement();
					}
				}
				else
					reader.skipCurrentElement();
			}
		}
		else if (reader.name() == "Organisation" && reader.attributes().hasAttribute("type")) {
			if (reader.attributes().value("type").toString() == "NationalFederation") {
				while(reader.readNextStartElement()) {
					if (reader.name() == "Country") {
						if (reader.attributes().hasAttribute("code"))
							s.countryCode = reader.attributes().value("code").toString();
						s.countryName = reader.readElementText();
					}
					else if (reader.name() == "Id" && reader.attributes().hasAttribute("type") && reader.attributes().value("type").toString() == "IOF")
						s.clubIdIof = reader.readElementText().toInt();
					else
						reader.skipCurrentElement();
				}
			}
			else if (reader.attributes().value("type").toString() == "Club") {
				while(reader.readNextStartElement()) {
					if (reader.name() == "Name") {
						if (reader.attributes().hasAttribute("code"))
							s.clubCode = reader.attributes().value("code").toString();
						s.clubName = reader.readElementText();
					}
					else if (reader.name() == "Id" && reader.attributes().hasAttribute("type") && reader.attributes().value("type").toString() == "IOF")
						s.clubIdIof = reader.readElementText().toInt();
					else
						reader.skipCurrentElement();
				}
			}
			else
				reader.skipCurrentElement();
		}
		else if (reader.name() == "ControlCard" && reader.attributes().hasAttribute("punchingSystem") && reader.attributes().value("punchingSystem").toString() == "SI") {
			s.siNumber = reader.readElementText().toInt();
		}
		else if (reader.name() == "Class") {
			while(reader.readNextStartElement()) {
				if (reader.name() == "Name")
					s.className = reader.readElementText();
				else if (reader.name() == "ShortName")
					s.classNameShort = reader.readElementText();
				else
					reader.skipCurrentElement();
			}
		}
		else if (reader.name() == "RaceNumber")
			s.enterRaces.insert(reader.readElementText().toInt());
		else if (reader.name() == "Leg")
			s.legNumber = reader.readElementText().toInt();
		else if (reader.name() == "Extensions") {
			while(reader.readNextStartElement()) {
				if (reader.name() == "Note")
					s.noteOris = reader.readElementText();
				else
					reader.skipCurrentElement();
			}
		}
		else
			reader.skipCurrentElement();
	}

	return result;
}


bool XmlImporter::importEntries(QXmlStreamReader &reader, const XmlCreators creator)
{
	QMap<QString, int> classes_map; // classes.name->classes.id
	qf::core::sql::Query q;
	q.exec("SELECT id, name FROM classes", qf::core::Exception::Throw);
	while(q.next()) {
		classes_map[q.value(1).toString()] = q.value(0).toInt();
	}
	if (classes_map.size() == 0){
		qfError() << "Undefined classes for entries";
		return false;
	}

	QMap<int, QString> clubs_map;
	if (creator == XmlCreators::Eventor) {
		q.exec("SELECT abbr, importId FROM clubs", qf::core::Exception::Throw);
		while(q.next()) {
			clubs_map[q.value(1).toInt()] = q.value(0).toString();
		}
		if (clubs_map.size() == 0){
			qfError() << "Undefined clubs for entries";
			return false;
		}
	}

	int selected_race = 0; // if entries has more races in (defined in Event), selected race
	// load from XML & insert to db
	int items_processed = 0;
	qf::core::sql::Transaction transaction;
	while(reader.readNextStartElement()) {
		if(reader.name() == "PersonEntry") {
			SPerson person;
			if (readPersonNode(person,reader, creator)) {

				if (selected_race != 0 && person.enterRaces.size() > 0 && !person.enterRaces.contains(selected_race)) {
					qfInfo() << "Skip entry" << person.nameGiven << person.nameFamily << "- not participate in this race";
				}
				else {
					Competitors::CompetitorDocument doc;
					doc.setEmitDbEventsOnSave(false);
					doc.loadForInsert();
					int class_id = classes_map.value(person.className);
					if(class_id == 0) {
						class_id = classes_map.value(person.classNameShort);
						if(class_id == 0) {
							qfError() << "Undefined class name:" << person.className << "for runner:" << person.nameGiven << " " << person.nameFamily;
							transaction.rollback();
							return false;
						}
					}
					doc.setValue("classId", class_id);
					if(person.siNumber > 0) {
						doc.setSiid(person.siNumber);
					}
					doc.setValue("firstName", person.nameGiven);
					doc.setValue("lastName", person.nameFamily);
					if (creator == XmlCreators::Oris) {
						doc.setValue("registration", person.regCz);
						doc.setValue("note", person.noteOris);
						if (person.orisId.has_value())
							doc.setValue("importId",person.orisId.value());
					}
					else if (creator == XmlCreators::Eventor) {
						if (!person.clubIdIof.has_value()) {
							// without club & federation
							doc.setValue("registration",person.nationalityCode);
						}
						else {
							QString club_abbr = (person.clubIdIof.has_value()) ? clubs_map[person.clubIdIof.value()] : "";
							if (!club_abbr.isEmpty())
								doc.setValue("registration",club_abbr);
							if (!person.clubName.isEmpty())
								doc.setValue("club",person.clubName);
							else if (!person.countryName.isEmpty())
								doc.setValue("club",person.countryName);
						}
						doc.setValue("country",person.nationalityName);
						if (person.iofId.has_value())
							doc.setValue("importId",person.iofId.value());
					}
					if (person.iofId.has_value())
						doc.setValue("iofId", person.iofId.value());
					doc.save();
					items_processed++;
				}
			}
			else
				qfWarning() << "Failed to read runner entry on pos" << items_processed << " [" << person.className << ","
							<< person.nameFamily << "," << person.nameGiven << ","
							<< ((person.iofId.has_value()) ? person.iofId.value(): -1 ) << "," << person.regCz << ","
							<< person.countryCode << "]";
		}
		else if (reader.name() == "Event") {
			QMap <QString, int> races;
			while(reader.readNextStartElement()) {
				if (reader.name() == "Race") {
					QString name, date;
					int number;
					while(reader.readNextStartElement()) {
						if (reader.name() == "Name")
							name = reader.readElementText();
						else if (reader.name() == "RaceNumber")
							number = reader.readElementText().toInt();
						else if (reader.name() == "StartTime") {
							while(reader.readNextStartElement()) {
								if (reader.name() == "Date")
									date = reader.readElementText();
								else
									reader.skipCurrentElement();
							}
						}
						else
							reader.skipCurrentElement();
					}
					QString race = QString("[%1] %2 %3").arg(number).arg(name).arg(date);
					races[race] = number;
				}
				else
					reader.skipCurrentElement();
			}
			if (races.size() > 0) {
				// when defined some races ...
				if (races.size() == 1)
					selected_race = races.first();
				else {
					QStringList items;
					auto it = races.begin();
					while (it != races.end()) {
						items << it.key();
						it++;
					}
					bool ok;
					QString item = QInputDialog::getItem(qf::qmlwidgets::framework::MainWindow::frameWork(), tr("Select which race import)"),
														 tr("Races:"), items, 0, false, &ok);
					if (ok && !item.isEmpty())
						selected_race = races[item];
					else
						return false;
				}
			}
		}
		else
			reader.skipCurrentElement();
	}

	if (items_processed > 0) {
		transaction.commit();
		qfInfo() << "Imported entry for" << items_processed << "runners.";
		getPlugin<EventPlugin>()->emitReloadDataRequest();
		getPlugin<EventPlugin>()->emitDbEvent(Event::EventPlugin::DBEVENT_COMPETITOR_COUNTS_CHANGED);
	}

	return items_processed > 0;
}

bool XmlImporter::importStartlist(QXmlStreamReader &reader, const XmlCreators creator)
{
	// load from XML & insert to db
	int items_processed = 0;
	qf::core::sql::Transaction transaction;
	while(reader.readNextStartElement()) {
		if(reader.name() == "PersonEntry") {
			SPerson person;
			if (readPersonNode(person,reader, creator)) {
				//insert to db
				items_processed++;
			}
		}
		else
			reader.skipCurrentElement();
	}

	if (items_processed > 0) {
		transaction.commit();
		qfInfo() << "Imported entry for" << items_processed << "runners.";
		getPlugin<EventPlugin>()->emitReloadDataRequest();
		getPlugin<EventPlugin>()->emitDbEvent(Event::EventPlugin::DBEVENT_COMPETITOR_COUNTS_CHANGED);
	}

	return items_processed > 0;
}

bool XmlImporter::importClasses(QXmlStreamReader &reader, const XmlCreators creator)
{
	// load data from XML
	qf::core::sql::Transaction transaction;
	int items_processed = 0;
	while(reader.readNextStartElement()) {
		if(reader.name() == "Class") {
			QString class_name;
			int class_id = 0;
			while(reader.readNextStartElement()) {
				if (reader.name() == "Name")
					class_name = reader.readElementText();
				else if (reader.name() == "Id" && reader.attributes().hasAttribute("type") && reader.attributes().value("type").toString() == "ORIS" && creator == XmlCreators::Oris)
					class_id = reader.readElementText().toInt();
				else if (reader.name() == "Id" && reader.attributes().hasAttribute("type") && reader.attributes().value("type").toString() == "IOF" && creator == XmlCreators::Eventor)
					class_id = reader.readElementText().toInt();
				else
					reader.skipCurrentElement();
			}

			// insert to db
			if (class_id != 0 && !class_name.isEmpty())	{
				try {
					Classes::ClassDocument doc;
					qfInfo() << "adding class id:" << class_id << "name:" << class_name;
					doc.loadForInsert();
					doc.setValue("id", class_id);
					doc.setValue("name", class_name);
					doc.save();
					items_processed++;
				}
				catch (const qf::core::Exception &e) {
					qfError() << "Import Class " << class_name << " ERROR:" << e.message();
				}
			}
		}
		else
			reader.skipCurrentElement();
	}

	if (items_processed > 0) {
		transaction.commit();
		qfInfo() << "Imported" << items_processed << "classes.";
//		getPlugin<Classes::ClassesPlugin>()->emitReloadDataRequest(); -- not exists
	}

	return (items_processed > 0);
}

QString XmlImporter::genFakeCzClubAbbr(QString country)
{
	if (country.isEmpty())
		return "";
	QString c = QString(country[0]);
	int pos = fakeCzClubMap[c];
	fakeCzClubMap[c]++;
	QString result = QString("%1%2").arg(c).arg(pos,2,36,QLatin1Char('0')).toUpper();
	if (result == country)
		return genFakeCzClubAbbr(country);
	else
		return result;
}


bool XmlImporter::importClubs(QXmlStreamReader &reader, const XmlCreators creator)
{
	// load data from XML
	qf::core::sql::Transaction transaction;
	qf::core::sql::Query q;
	q.exec("DELETE FROM clubs", qf::core::Exception::Throw);
	q.prepare("INSERT INTO clubs (name, abbr, importId) VALUES (:name, :abbr, :importId)", qf::core::Exception::Throw);
	int items_processed = 0;
	if (creator == XmlCreators::Eventor)
		fakeCzClubMap.clear();
	while(reader.readNextStartElement()) {
		if(reader.name() == "Organisation" && reader.attributes().hasAttribute("type")) {
			QString name;

			// ORIS
			QString abbr_cz;
			QString code_cz;
			int id_cz = -1;
			// Eventor
			int id_iof = -1;
			QString country;
			QString country_code;
			bool federation_iof = false;

			if (reader.attributes().value("type").toString() == "NationalFederation") {
				federation_iof = true;
				while(reader.readNextStartElement()) {
					if (reader.name() == "Id" && reader.attributes().hasAttribute("type")&& reader.attributes().value("type").toString() == "IOF")
						id_iof =  reader.readElementText().toInt();
					else if (reader.name() == "Name")
						name = reader.readElementText();
					else if (reader.name() == "Country") {
						if (reader.attributes().hasAttribute("code"))
							country_code = reader.attributes().value("code").toString();
						country = reader.readElementText();
					}
					else
						reader.skipCurrentElement();
				}
			}
			else if (reader.attributes().value("type").toString() == "Club" || reader.attributes().value("type").toString() == "Other") {
				while(reader.readNextStartElement()) {
					if (reader.name() == "Id" && reader.attributes().hasAttribute("type")&& reader.attributes().value("type").toString() == "ORIS")
						id_cz =  reader.readElementText().toInt();
					else if (reader.name() == "Id" && reader.attributes().hasAttribute("type")&& reader.attributes().value("type").toString() == "IOF")
						id_iof =  reader.readElementText().toInt();
					else if (reader.name() == "Name") {
						if (reader.attributes().hasAttribute("code"))
							code_cz = reader.attributes().value("code").toString();
						name = reader.readElementText();
					}
					else if (reader.name() == "Extensions")
					{
						while(reader.readNextStartElement()) {
							if (reader.name() == "Abbreviation")
								abbr_cz = reader.readElementText();
							else
								reader.skipCurrentElement();
						}
					}
					else if (reader.name() == "Country") {
						if (reader.attributes().hasAttribute("code"))
							country_code = reader.attributes().value("code").toString();
						reader.readElementText(); // need to be here, for next element
					}
					else
						reader.skipCurrentElement();
				}
			}
			else
				reader.skipCurrentElement();

			// insert to db
			if (!name.isEmpty() || (federation_iof && !country.isEmpty())) {
				if (creator == XmlCreators::Oris) {
					q.bindValue(":name", name);
					q.bindValue(":abbr", abbr_cz);
					q.bindValue(":importId", id_cz);
				}
				else if (creator == XmlCreators::Eventor) {
					if (federation_iof) {
						q.bindValue(":abbr", country_code);
						q.bindValue(":name", country);
					}
					else {
						q.bindValue(":name", name);
						q.bindValue(":abbr", genFakeCzClubAbbr(country_code));
					}
					q.bindValue(":importId", id_iof);
				}
				q.exec(qf::core::Exception::Throw);
				items_processed++;
			}
		}
		else
			reader.skipCurrentElement();
	}
	if (items_processed > 0) {
		transaction.commit();
		qfInfo() << "Imported" << items_processed << "clubs.";
	}

	if (creator == XmlCreators::Eventor){
		int max_item = -1;
		QString key;
		auto it = fakeCzClubMap.begin();
		while (it != fakeCzClubMap.end()) {
			if (it.value() > max_item) {
				max_item = it.value();
				key = it.key();
			}
			it++;
		}
		qfInfo() << "Maximum clubs from one country was" << max_item << "in" << key;
		fakeCzClubMap.clear();
	}
	return items_processed > 0;
}

bool XmlImporter::importRegistration(QXmlStreamReader &reader, const XmlCreators creator)
{
	QMap<int, QString> clubs_map;
	qf::core::sql::Query q;
	if (creator == XmlCreators::Eventor) {
		q.exec("SELECT abbr, importId FROM clubs", qf::core::Exception::Throw);
		while(q.next()) {
			clubs_map[q.value(1).toInt()] = q.value(0).toString();
		}
		if (clubs_map.size() == 0){
			qfError() << "Undefined clubs for registration";
			return false;
		}
	}
	qf::core::sql::Transaction transaction;
	q.exec("DELETE FROM registrations", qf::core::Exception::Throw);
	q.prepare("INSERT INTO registrations (firstName, lastName, registration, licence, clubAbbr, country, siId, importId) VALUES (:firstName, :lastName, :registration, :licence, :clubAbbr, :country, :siId, :importId)", qf::core::Exception::Throw);
	// load data from XML
	int items_processed = 0;
	while(reader.readNextStartElement()) {
		if(reader.name() == "Competitor") {
			SPerson person;
			if (readPersonNode(person,reader, creator)) {
				q.bindValue(":firstName", person.nameGiven);
				q.bindValue(":lastName", person.nameFamily);
				if (creator == XmlCreators::Oris) {
					q.bindValue(":licence", "");
					if (person.orisId.has_value())
						q.bindValue(":importId", person.orisId.value());
					else
						q.bindValue(":importId", 0);
					q.bindValue(":siId", person.siNumber);
					if(!person.regCz.isEmpty()) {
						q.bindValue(":registration", person.regCz);
						q.bindValue(":clubAbbr", person.regCz.mid(0, 3));
					}
				}
				else if (creator == XmlCreators::Eventor) {
					QString club_abbr = (person.clubIdIof.has_value()) ? clubs_map.value(person.clubIdIof.value()) : "";
					if(!club_abbr.isEmpty()) {
						q.bindValue(":clubAbbr", club_abbr);
						q.bindValue(":registration", person.regCz);
					}
					if (person.iofId.has_value())
						q.bindValue(":importId", person.iofId.value());
					else
						q.bindValue(":importId", 0);
				}
				q.bindValue(":country",person.nationalityCode);
				q.exec(qf::core::Exception::Throw);
				items_processed++;
			}
			else
				qfWarning() << "Failed to read runner registration on pos" << items_processed;
		}
		else
			reader.skipCurrentElement();
	}

	if (items_processed > 0) {
		transaction.commit();
		qfInfo() << "Imported" << items_processed << "registered runners.";
		getPlugin<EventPlugin>()->emitDbEvent(EventPlugin::DBEVENT_REGISTRATIONS_IMPORTED, QVariant(), true);
	}

	return items_processed > 0;
}

bool XmlImporter::importXML30()
{
	qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
	QString fn = qfd::FileDialog::getOpenFileName(fwk, tr("Open XML Entry file"), QString(), tr("IOF XML v3 files (*.xml)"));
	if(fn.isEmpty())
		return false;
	QFile file(fn);
	if(!file.open(QFile::ReadOnly | QFile::Text))
		return false;

	QXmlStreamReader reader(&file);
	if (reader.readNextStartElement()) {
		XmlCreators creator = XmlCreators::QuickEvent;
		if (reader.attributes().hasAttribute("creator")) {
			QString name = reader.attributes().value("creator").toString();
			if (name == "ORIS")
				creator = XmlCreators::Oris;
			else if (name == "Eventor")
				creator = XmlCreators::Eventor;
		}

		if (reader.name() == "EntryList")
			return importEntries(reader, creator);
		else if (reader.name() == "StartList")
			return importStartlist(reader, creator);
		else if (reader.name() == "ClassList")
			return importClasses(reader, creator);
		else if (reader.name() == "OrganisationList") // clubs
			return importClubs(reader, creator);
		else if (reader.name() == "CompetitorList") // registration
			return importRegistration(reader, creator);
		else if (reader.name() == "EventList")
			return false; // not used yet - maybe for race info, maybe for racelist (WMOC)
		else
			qfd::MessageBox::showWarning(fwk, QString(tr("Unsuported IOF XML 3.0 type (%1)")).arg(reader.name()));
	}
	return false;
}

