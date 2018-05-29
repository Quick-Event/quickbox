#include "chooseoriseventdialog.h"
#include "orisimporter.h"

#include <Event/eventplugin.h>
#include <Classes/classesplugin.h>
#include <Classes/classdocument.h>
#include <Competitors/competitorsplugin.h>
#include <Competitors/competitordocument.h>

#include <qf/qmlwidgets/framework/mainwindow.h>
#include <qf/qmlwidgets/dialogs/dialog.h>
#include <qf/qmlwidgets/dialogs/getiteminputdialog.h>
#include <qf/qmlwidgets/dialogs/messagebox.h>
#include <qf/qmlwidgets/framework/plugin.h>
#include <qf/qmlwidgets/dialogbuttonbox.h>
#include <qf/qmlwidgets/htmlviewwidget.h>

#include <qf/core/network/networkaccessmanager.h>
#include <qf/core/network/networkreply.h>
#include <qf/core/log.h>
#include <qf/core/sql/query.h>
#include <qf/core/sql/transaction.h>
#include <qf/core/utils/fileutils.h>
#include <qf/core/utils/htmlutils.h>

#include <qf/core/assert.h>
#include <qf/core/log.h>

#include <QDate>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>
#include <QPushButton>
#include <QTime>
#include <QUrl>

static Event::EventPlugin* eventPlugin()
{
	qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
	auto *plugin = qobject_cast<Event::EventPlugin*>(fwk->plugin("Event"));
	QF_ASSERT_EX(plugin != nullptr, "Bad event plugin!");
	return plugin;
}
/*
static Event::EventPlugin* classesPlugin()
{
	qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
	auto *plugin = qobject_cast<Event::EventPlugin*>(fwk->plugin("Event"));
	QF_ASSERT_EX(plugin != nullptr, "Bad event plugin!");
	return plugin;
}

static Competitors::CompetitorsPlugin* competitorsPlugin()
{
	qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
	auto *plugin = qobject_cast<Competitors::CompetitorsPlugin*>(fwk->plugin("Competitors"));
	QF_ASSERT_EX(plugin != nullptr, "Bad Competitors plugin!");
	return plugin;
}
*/

void OrisImporter::saveJsonBackup(const QString &fn, const QJsonDocument &jsd)
{
	QString dir = qf::core::utils::FileUtils::osTempDir() + '/' + "quickevent";
	qf::core::utils::FileUtils::ensurePath(dir);
	QFile f(dir + '/' + fn + ".json");
	if(f.open(QFile::WriteOnly)) {
		QByteArray ba = jsd.toJson();
		f.write(ba);
	}
}

static QJsonDocument load_offline_json(const QString &fn)
{
	QJsonDocument ret;
	QString dir = qf::core::utils::FileUtils::osTempDir() + '/' + "quickevent";
	QFile f(dir + '/' + fn + ".load.json");
	if(f.open(QFile::ReadOnly)) {
		ret = QJsonDocument::fromJson(f.readAll());
	}
	return ret;
}

OrisImporter::OrisImporter(QObject *parent)
	: QObject(parent)
{

}

qf::core::network::NetworkAccessManager *OrisImporter::networkAccessManager()
{
	if(!m_networkAccessManager) {
		m_networkAccessManager = new qf::core::network::NetworkAccessManager(this);
	}
	return m_networkAccessManager;
}

void OrisImporter::getJsonAndProcess(const QUrl &url, QObject *context, std::function<void (const QJsonDocument &)> process_call_back)
{
	auto *manager = networkAccessManager();
	qf::core::network::NetworkReply *reply = manager->get(url);
	qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
	connect(context, &QObject::destroyed, reply, &qf::core::network::NetworkReply::deleteLater);
	connect(reply, &qf::core::network::NetworkReply::downloadProgress, fwk, &qf::qmlwidgets::framework::MainWindow::showProgress);
	connect(reply, &qf::core::network::NetworkReply::finished, context, [reply, process_call_back](bool get_ok) {
		qfInfo() << "Get:" << reply->url().toString() << "OK:" << get_ok;
		qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
		if(get_ok) {
			QJsonParseError err;
			QJsonDocument jsd = QJsonDocument::fromJson(reply->data(), &err);
			if(err.error != QJsonParseError::NoError) {
				qfError() << reply->data();
				qf::qmlwidgets::dialogs::MessageBox::showError(fwk, tr("JSON document parse error: %1 at: %2 near: %3")
															   .arg(err.errorString())
															   .arg(err.offset)
															   .arg(reply->data().mid(err.offset, 50).constData()));
				return;
			}
			process_call_back(jsd);
		}
		else {
			qf::qmlwidgets::dialogs::MessageBox::showError(fwk, "http get error on: " + reply->url().toString() + ", " + reply->errorString());
		}
		reply->deleteLater();
	});
}

void OrisImporter::getTextAndProcess(const QUrl &url, QObject *context, std::function<void (const QByteArray &)> process_call_back)
{
	auto *manager = networkAccessManager();
	qf::core::network::NetworkReply *reply = manager->get(url);
	qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
	connect(context, &QObject::destroyed, reply, &qf::core::network::NetworkReply::deleteLater);
	connect(reply, &qf::core::network::NetworkReply::downloadProgress, fwk, &qf::qmlwidgets::framework::MainWindow::showProgress);
	connect(reply, &qf::core::network::NetworkReply::finished, context, [reply, process_call_back](bool get_ok) {
		qfInfo() << "Get:" << reply->url().toString() << "OK:" << get_ok;
		qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
		if(get_ok) {
			process_call_back(reply->data());
		}
		else {
			qf::qmlwidgets::dialogs::MessageBox::showError(fwk, "http get error on: " + reply->url().toString() + ", " + reply->errorString());
		}
		reply->deleteLater();
	});
}

void OrisImporter::syncCurrentEventEntries()
{
	qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
	//if(!qf::qmlwidgets::dialogs::MessageBox::askYesNo(fwk, tr("All runners entries imported from Oris will be synchronized, manual changes will be lost!")))
	int oris_id = eventPlugin()->eventConfig()->importId();
	if(oris_id == 0) {
		qf::qmlwidgets::dialogs::MessageBox::showError(fwk, tr("Cannot find Oris import ID."));
		return;
	}
	importEventOrisEntries(oris_id);
}

void OrisImporter::syncRelaysEntries()
{
	qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
	if(!eventPlugin()->eventConfig()->isRelays()) {
		qf::qmlwidgets::dialogs::MessageBox::showError(fwk, tr("Not relays event."));
		return;
	}
	int oris_id = eventPlugin()->eventConfig()->importId();
	if(oris_id == 0) {
		qf::qmlwidgets::dialogs::MessageBox::showError(fwk, tr("Cannot find Oris import ID."));
		return;
	}
	QUrl url(QString("https://oris.orientacnisporty.cz/ExportPrihlasek?id=%1").arg(oris_id));
	getTextAndProcess(url, this, [](const QByteArray &data) {
		qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
		try {
			//qfInfo() << data;
			qf::core::sql::Transaction transaction;
			qf::core::sql::Query q;

			q.execThrow("UPDATE competitors SET importId=1 WHERE importId IS NOT NULL");
			q.execThrow("UPDATE runs SET importId=1 WHERE importId IS NOT NULL");
			q.execThrow("UPDATE relays SET importId=1 WHERE importId IS NOT NULL");

			QMap<QString, int> class_ids;
			q.execThrow("SELECT id, name FROM classes");
			while(q.next())
				class_ids[q.value(1).toString()] = q.value(0).toInt();

			QTextStream ts(data);
			ts.setCodec("cp1250");
			enum E1 {
				Club = 4,
				RelayPos = 3,
				ClassName = 11,
				Reg = 8,
				Name = 28,
				SI = 9,
				//Competitor = Reg + Name + SI,
			};
			while(!ts.atEnd()) {
				QString ln = ts.readLine();
				QStringView line(ln);
				int n = 0;

				QStringView club = line.mid(n, Club).trimmed(); n += Club;
				if(club.isEmpty())
					continue;
				QStringView pos = line.mid(n, RelayPos).trimmed(); n += RelayPos;
				QString class_name = line.mid(n, ClassName).trimmed().toString(); n += ClassName;

				int class_id = class_ids.value(class_name);
				if(class_id == 0) {
					qfError() << "Invalid class name" << class_name;
					continue;
				}

				qfInfo() << club << pos << class_name << class_id;

				QString relay_name = (club.toString() + ' ' + pos.toString()).trimmed();
				q.execThrow("SELECT id FROM relays WHERE"
							" name='" + relay_name + "'"
							" AND classId=" + QString::number(class_id));
				int relay_id;
				if(q.next()) {
					relay_id = q.value(0).toInt();
					q.execThrow("UPDATE relays SET importId=2 WHERE id=" + QString::number(relay_id));
				}
				else {
					q.execThrow("INSERT INTO relays (classId, name, importId) VALUES ("
								+ QString::number(class_id) + ", "
								+ "'" + relay_name + "', "
								+ "2"
								+ ")");
					relay_id = q.lastInsertId().toInt();
				}

				int leg = 0;
				while(n < line.size()) {
					leg++;
					QString reg = line.mid(n, Reg).trimmed().toString(); n += Reg;
					QString name = line.mid(n, Name).trimmed().toString(); n += Name;
					QString last_name = name.section(' ', 0, 0);
					QString first_name = name.section(' ', 1);
					int si = line.mid(n, SI).trimmed().toString().toInt(); n += SI;

					if(reg.isEmpty() && name.isEmpty())
						continue;

					qfInfo() << '\t' << leg << last_name << first_name << reg << si;

					int competitor_id = 0;
					{
						q.execThrow("SELECT id FROM competitors WHERE"
									" firstName='" + first_name + "'"
									" AND lastName='" + last_name + "'"
									" AND registration='" + reg + "'");
						if(q.next()) {
							competitor_id = q.value(0).toInt();
						}
						else {
							if(!reg.isEmpty()) {
								q.execThrow("SELECT id FROM competitors WHERE"
											" registration='" + reg + "'");
								if(q.next())
									competitor_id = q.value(0).toInt();
							}
							if(competitor_id == 0) {
								q.execThrow("SELECT id FROM competitors WHERE"
											" firstName='" + first_name + "'"
											" AND lastName='" + last_name + "'");
								if(q.next())
									competitor_id = q.value(0).toInt();
							}
							if(competitor_id == 0) {
								if(reg.isEmpty()) {
									q.execThrow("INSERT INTO competitors (lastName) VALUES ('" + last_name + "')");
									competitor_id = q.lastInsertId().toInt();
								}
								else {
									q.execThrow("INSERT INTO competitors (registration) VALUES ('" + reg + "')");
									competitor_id = q.lastInsertId().toInt();
								}
							}
						}
						Q_ASSERT(competitor_id > 0);
						q.execThrow("UPDATE competitors SET"
									" firstName='" + first_name + "',"
									" lastName='" + last_name + "',"
									" registration='" + reg + "',"
									" siid=" + QString::number(si) + ","
									" importId=2"
									" WHERE id=" + QString::number(competitor_id)
									);
					}
					int run_id;
					{
						q.execThrow("SELECT id FROM runs WHERE"
									" relayId=" + QString::number(relay_id) + ""
									" AND leg=" + QString::number(leg));
						if(q.next()) {
							run_id = q.value(0).toInt();
						}
						else {
							q.execThrow("INSERT INTO runs (relayId) VALUES (" + QString::number(relay_id) + ")");
							run_id = q.lastInsertId().toInt();
						}
						q.execThrow("UPDATE runs SET"
									" competitorId=" + QString::number(competitor_id) + ","
									" stageId=1,"
									" leg=" + QString::number(leg) + ","
									" siid=" + QString::number(si) + ","
									" importId=2"
									" WHERE id=" + QString::number(competitor_id)
									);
					}
				}
			}
			q.execThrow("DELETE FROM runs WHERE importId=1");
			q.execThrow("DELETE FROM competitors WHERE importId=1");
			q.execThrow("DELETE FROM relays WHERE importId=1");
			transaction.commit();
			qf::qmlwidgets::dialogs::MessageBox::showInfo(fwk, tr("Import finished successfully."));
		}
		catch (qf::core::Exception &e) {
			qf::qmlwidgets::dialogs::MessageBox::showException(fwk, e);
		}
	});
}

void OrisImporter::chooseAndImport()
{
	qfLogFuncFrame();
	qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
	ChooseOrisEventDialog dlg(this, fwk);
	if(!dlg.exec())
		return;
	int event_id = dlg.eventId();
	if(event_id > 0)
		importEvent(event_id);
}

static QString jsonObjectToFullName(const QJsonObject &data, const QString &field_name)
{
	QJsonObject jso = data.value(field_name).toObject();
	return jso.value(QStringLiteral("FirstName")).toString() + ' ' + jso.value(QStringLiteral("LastName")).toString();
}

void OrisImporter::importEvent(int event_id)
{
	QUrl url(QString("https://oris.orientacnisporty.cz/API/?format=json&method=getEvent&id=%1").arg(event_id));
	getJsonAndProcess(url, this, [this, event_id](const QJsonDocument &jsd) {
		qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
		try {
			saveJsonBackup("Event", jsd);
			QJsonObject data = jsd.object().value(QStringLiteral("Data")).toObject();;
			int stage_count = data.value(QStringLiteral("Stages")).toString().toInt();
			if(!stage_count)
				stage_count = 1;
			int sport_id = data.value(QStringLiteral("Sport")).toObject().value(QStringLiteral("ID")).toString().toInt();
			int discipline_id = data.value(QStringLiteral("Discipline")).toObject().value(QStringLiteral("ID")).toString().toInt();
			qfInfo() << "pocet etap:" << stage_count << "sport id:" << sport_id << "discipline id:" << discipline_id;
			//event_api.initEventConfig();
			//var cfg = event_api.eventConfig;
			QVariantMap ecfg;
			ecfg["stageCount"] = stage_count;
			ecfg["name"] = data.value(QStringLiteral("Name")).toString();
			ecfg["description"] = QString();
			ecfg["date"] = QDate::fromString(data.value(QStringLiteral("Date")).toString(), Qt::ISODate);
			ecfg["place"] = data.value(QStringLiteral("Place")).toString();
			ecfg["mainReferee"] = jsonObjectToFullName(data, QStringLiteral("MainReferee"));
			ecfg["director"] = jsonObjectToFullName(data, QStringLiteral("Director"));
			ecfg["sportId"] = sport_id;
			ecfg["disciplineId"] = discipline_id;
			ecfg["importId"] = event_id;
			ecfg["time"] = QTime::fromString(data.value(QStringLiteral("StartTime")).toString(), QStringLiteral("hh:mm"));
			if(!eventPlugin()->createEvent(QString(), ecfg))
				return;

			//QString event_name = eventPlugin()->eventName();
			qf::core::sql::Transaction transaction;

			int items_processed = 0;
			int items_count = 0;
			QJsonObject classes_o = data.value(QStringLiteral("Classes")).toObject();
			for(auto it = classes_o.constBegin(); it != classes_o.constEnd(); ++it) {
				items_count++;
			}
			Classes::ClassDocument doc;
			for(auto it = classes_o.constBegin(); it != classes_o.constEnd(); ++it) {
				QJsonObject class_o = it.value().toObject();
				int class_id = class_o.value(QStringLiteral("ID")).toString().toInt();
				QString class_name = class_o.value(QStringLiteral("Name")).toString();
				fwk->showProgress("Importing class: " + class_name, items_processed++, items_count);
				qfInfo() << "adding class id:" << class_id << "name:" << class_name;
				doc.loadForInsert();
				doc.setValue("id", class_id);
				doc.setValue("name", class_name);
				doc.save();
			}
			transaction.commit();
			fwk->hideProgress();
		}
		catch (qf::core::Exception &e) {
			qf::qmlwidgets::dialogs::MessageBox::showException(fwk, e);
		}
		importEventOrisEntries(event_id);
	});
}

static QVariantList create_html_table(const QString &title, const QStringList &flds, const QVariantList &rows)
{
	QVariantList div = QVariantList() << QStringLiteral("div");
	div.insert(div.length(), QVariantList() << QStringLiteral("h2") << title);
	QVariantList table = QVariantList() << QStringLiteral("table");
	QVariantList header = QVariantList() << QStringLiteral("tr");
	for(auto fld : flds)
		header.insert(header.length(), QVariantList() << QStringLiteral("th") << fld);
	table.insert(table.length(), header);
	table << rows;
	div.insert(div.length(), table);
	return div;
}

void OrisImporter::importEventOrisEntries(int event_id)
{
	if(eventPlugin()->eventConfig()->isRelays()) {
		qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
		qf::qmlwidgets::dialogs::MessageBox::showError(fwk, tr("Not supported for the relays event."));
		return;
	}
	QUrl url(QString("https://oris.orientacnisporty.cz/API/?format=json&method=getEventEntries&eventid=%1").arg(event_id));
	getJsonAndProcess(url, this, [](const QJsonDocument &jsd) {
		static const QString json_fn = "EventEntries";
		saveJsonBackup(json_fn, jsd);
		qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
		try {
			qf::core::sql::Query q;
			QMap<int, QString> classes_map; // classes.id->classes.name
			q.exec("SELECT id, name FROM classes", qf::core::Exception::Throw);
			while(q.next()) {
				classes_map[q.value(0).toInt()] = q.value(1).toString();
			}
			QMap<int, int> imported_competitors; // importId->id
			q.exec("SELECT id, importId FROM competitors WHERE importId>0 ORDER BY importId", qf::core::Exception::Throw);
			while(q.next()) {
				imported_competitors[q.value(1).toInt()] = q.value(0).toInt();
			}
			QJsonDocument jsd2 = load_offline_json(json_fn);
			if(jsd2.isNull())
				jsd2 = jsd;
			QJsonObject data = jsd2.object().value(QStringLiteral("Data")).toObject();;
			int items_processed = 0;
			int items_count = 0;
			for(auto it = data.constBegin(); it != data.constEnd(); ++it) {
				items_count++;
			}
			QList<Competitors::CompetitorDocument*> doc_lst;
			doc_lst.reserve(items_count);
			QSet<int> used_idsi;
			for(auto it = data.constBegin(); it != data.constEnd(); ++it) {
				QJsonObject competitor_o = it.value().toObject();
				Competitors::CompetitorDocument *doc = new Competitors::CompetitorDocument();
				doc_lst << doc;
				int import_id = competitor_o.value(QStringLiteral("ID")).toString().toInt();
				int competitor_id = imported_competitors.value(import_id);
				if(competitor_id > 0) {
					doc->load(competitor_id);
					imported_competitors.remove(import_id);
				}
				else {
					doc->loadForInsert();
				}
				QString siid_str = competitor_o.value(QStringLiteral("SI")).toString();
				bool ok;
				int siid = siid_str.toInt(&ok);
				//qfInfo() << "SI:" << siid, competitor_obj.ClassDesc, ' ', competitor_obj.LastName, ' ', competitor_obj.FirstName, "classId:", parseInt(competitor_obj.ClassID));
				QString note = competitor_o.value(QStringLiteral("Note")).toString();
				if(!ok) {
					note += " SI:" + siid_str;
				}
				QString requested_start = competitor_o.value(QStringLiteral("RequestedStart")).toString();
				if(!requested_start.isEmpty()) {
					note += " req. start: " + requested_start;
				}
				QString first_name = competitor_o.value(QStringLiteral("FirstName")).toString();
				QString last_name = competitor_o.value(QStringLiteral("LastName")).toString();
				if(first_name.isEmpty() && last_name.isEmpty()) {
					QString name = competitor_o.value(QStringLiteral("Name")).toString();
					if(!name.isEmpty()) {
						last_name = name.section(' ', 0, 0);
						first_name = name.section(' ', 1);
					}
				}
				QString reg_no = competitor_o.value(QStringLiteral("RegNo")).toString();
				fwk->showProgress("Importing: " + reg_no + ' ' + last_name + ' ' + first_name, items_processed, items_count);
				int class_id = competitor_o.value(QStringLiteral("ClassID")).toString().toInt();
				if(!classes_map.contains(class_id)) {
					qfWarning() << "class id:" << class_id << "not found in the class definitions";
					class_id = 0;
				}
				doc->setValue("classId", (class_id == 0)? QVariant(QVariant::Int): QVariant(class_id));
				if(siid > 0) {
					bool is_unique = !used_idsi.contains(siid);
					if(is_unique)
						used_idsi << siid;
					doc->setSiid(siid, false);
				}
				doc->setValue("firstName", first_name);
				doc->setValue("lastName", last_name);
				doc->setValue("registration", reg_no);
				doc->setValue("licence", competitor_o.value(QStringLiteral("Licence")).toString());
				doc->setValue("note", note);
				doc->setValue("importId", import_id);
				items_processed++;
			}
			for(int id : imported_competitors.values()) {
				Competitors::CompetitorDocument *doc = new Competitors::CompetitorDocument();
				doc_lst << doc;
				doc->load(id, doc->ModeDelete);
			}
			static const QStringList fields = QStringList()
											  << QStringLiteral("classId")
											  << QStringLiteral("className")
											  << QStringLiteral("lastName")
											  << QStringLiteral("firstName")
											  << QStringLiteral("registration")
											  << QStringLiteral("siId")
											  << QStringLiteral("licence")
											  << QStringLiteral("note")
											  << QStringLiteral("importId");
			QVariantList new_entries_rows;
			QVariantList edited_entries_rows;
			QVariantList deleted_entries_rows;
			auto field_string = [classes_map](Competitors::CompetitorDocument *doc, const QString fldn) {
				QString s;
				if(fldn == QLatin1String("className"))
					s = classes_map.value(doc->value(QStringLiteral("classId")).toInt());
				else
					s = doc->value(fldn).toString();
				return s;
			};
			for(Competitors::CompetitorDocument *doc : doc_lst) {
				QVariantList tr = QVariantList() << QStringLiteral("tr");
				if(doc->mode() == doc->ModeInsert) {
					for(QString fldn : fields) {
						auto td = QVariantList() << QStringLiteral("td") << field_string(doc, fldn);
						tr.insert(tr.length(), td);
					}
					new_entries_rows.insert(new_entries_rows.length(), tr);
				}
				else if(doc->mode() == doc->ModeEdit) {
					if(doc->isDirty()) {
						for(QString fldn : fields) {
							static QVariantMap green_attrs;
							if(green_attrs.isEmpty())
								green_attrs["bgcolor"] = QStringLiteral("khaki");
							auto td = QVariantList() << QStringLiteral("td");
							if(fldn != QLatin1String("className") && doc->isDirty(fldn))
								td << green_attrs;
							td << field_string(doc, fldn);
							tr.insert(tr.length(), td);
						}
						edited_entries_rows.insert(edited_entries_rows.length(), tr);
					}
				}
				else if(doc->mode() == doc->ModeDelete) {
					for(QString fldn : fields) {
						auto td = QVariantList() << QStringLiteral("td") << field_string(doc, fldn);
						tr.insert(tr.length(), td);
					}
					deleted_entries_rows.insert(new_entries_rows.length(), tr);
				}
			}
			QVariantList html_body = QVariantList() << QStringLiteral("body");
			html_body.insert(html_body.length(), QVariantList() << QStringLiteral("body"));
			html_body.insert(html_body.length(), create_html_table(tr("New entries"), fields, new_entries_rows));
			html_body.insert(html_body.length(), create_html_table(tr("Edited entries"), fields, edited_entries_rows));
			html_body.insert(html_body.length(), create_html_table(tr("Deleted entries"), fields, deleted_entries_rows));
			fwk->hideProgress();
			qf::core::utils::HtmlUtils::FromHtmlListOptions opts;
			opts.setDocumentTitle(tr("Oris import report"));
			QString html = qf::core::utils::HtmlUtils::fromHtmlList(html_body, opts);
#ifdef Q_OS_LINUX_NNNNN
			QFile f("/tmp/1.html");
			if(f.open(QFile::WriteOnly)) {
				f.write(html.toUtf8());
			}
#endif
			qf::qmlwidgets::dialogs::Dialog dlg(QDialogButtonBox::Save | QDialogButtonBox::Cancel, fwk);
			qf::qmlwidgets::DialogButtonBox *bbx = dlg.buttonBox();
			QPushButton *bt_no_drops = new QPushButton(tr("Save without drops"));
			bool no_drops = false;
			connect(bt_no_drops, &QPushButton::clicked, [&no_drops]() {
				no_drops = true;
			});
			bbx->addButton(bt_no_drops, QDialogButtonBox::AcceptRole);
			auto *w = new qf::qmlwidgets::HtmlViewWidget();
			dlg.setCentralWidget(w);
			w->setHtmlText(html);
			if(dlg.exec()) {
				qf::core::sql::Transaction transaction;
				QMap<int, int> cid_sid_changes; // competitorId->siId
				const auto SIID = QStringLiteral("siId");
				for(Competitors::CompetitorDocument *doc : doc_lst) {
					if(doc->mode() == doc->ModeInsert) {
						doc->save();
						int siid = doc->value(SIID).toInt();
						cid_sid_changes[doc->dataId().toInt()] = siid;
					}
					else if(doc->mode() == doc->ModeEdit) {
						if(doc->isDirty()) {
							int siid = doc->value(SIID).toInt();
							cid_sid_changes[doc->dataId().toInt()] = siid;
							doc->save();
						}
					}
					else if(doc->mode() == doc->ModeDelete) {
						if(!no_drops)
							doc->drop();
					}
				}
				cid_sid_changes.remove(0);
				int stage_cnt = eventPlugin()->stageCount();
				for (int stage_id = 1; stage_id <= stage_cnt; ++stage_id) {
					QMap<int, int> sid_cid_map; // siId->competitorId
					q.exec("SELECT competitorId, siId FROM runs WHERE siId IS NOT NULL AND stageId=" QF_IARG(stage_id), qf::core::Exception::Throw);
					// take all SI->competitor_id assignments for this stage
					while(q.next()) {
						int cid = q.value(0).toInt();
						int sid = q.value(1).toInt();
						if(sid > 0)
							sid_cid_map[sid] = cid;
					}
					{
						// reply siid_changes changes in si_map
						QMapIterator<int, int> it(cid_sid_changes);
						while(it.hasNext()) {
							it.next();
							int competitor_id = it.key();
							int si_id = it.value();
							if(si_id > 0)
								sid_cid_map[si_id] = competitor_id;
						}
					}
					{
						// delete duplicit competitor->SI assignmets
						QMutableMapIterator<int, int> it(cid_sid_changes);
						while(it.hasNext()) {
							it.next();
							int cid = it.key();
							int sid = it.value();
							if(sid > 0) {
								int unique_siid_competitor_id = sid_cid_map.value(sid);
								if(unique_siid_competitor_id != cid) {
									qfInfo() << "SI:" << sid << "is duplicit in stage:" << stage_id;
									it.setValue(0);
								}
							}
						}
					}
					{
						// write SI changes to runs table
						q.prepare("UPDATE runs SET siId=:siId WHERE competitorId=:competitorId AND stageId=" QF_IARG(stage_id), qf::core::Exception::Throw);
						QMapIterator<int, int> it(cid_sid_changes);
						while(it.hasNext()) {
							it.next();
							int competitor_id = it.key();
							int si_id = it.value();
							qfDebug() << "stage:" << stage_id << "saving SI:" << si_id << "for competitor id:" << competitor_id;
							q.bindValue(QStringLiteral(":competitorId"), competitor_id);
							q.bindValue(QStringLiteral(":siId"), (si_id > 0)? si_id: QVariant(QVariant::Int));
							q.exec(qf::core::Exception::Throw);
						}
					}
				}
				transaction.commit();
			}
			qDeleteAll(doc_lst);
			emit eventPlugin()->reloadDataRequest();
		}
		catch (qf::core::Exception &e) {
			qf::qmlwidgets::dialogs::MessageBox::showException(fwk, e);
		}
	});
}

void OrisImporter::importRegistrations()
{
	int sport_id = eventPlugin()->eventConfig()->sportId();
	int year = QDate::currentDate().year();
	QUrl url(QString("https://oris.orientacnisporty.cz/API/?format=json&method=getRegistration&sport=%1&year=%2").arg(sport_id).arg(year));
	getJsonAndProcess(url, this, [](const QJsonDocument &jsd) {
		saveJsonBackup("Registrations", jsd);
		qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
		QJsonObject data = jsd.object().value(QStringLiteral("Data")).toObject();;
		// import clubs
		int items_processed = 0;
		int items_count = 0;
		for(auto it = data.constBegin(); it != data.constEnd(); ++it) {
			items_count++;
		}
		fwk->showProgress(tr("Importing registrations"), 1, items_count);
		try {
			qf::core::sql::Transaction transaction;
			qf::core::sql::Query q;
			q.exec("DELETE FROM registrations", qf::core::Exception::Throw);
			q.prepare("INSERT INTO registrations (firstName, lastName, registration, licence, clubAbbr, siId, importId) VALUES (:firstName, :lastName, :registration, :licence, :clubAbbr, :siId, :importId)", qf::core::Exception::Throw);
			for(auto it = data.constBegin(); it != data.constEnd(); ++it) {
				QJsonObject obj = it.value().toObject();
				//qfInfo() << obj.value("LastName") << obj.value("SI") << obj.value("UserID");
 				QString reg = obj.value(QStringLiteral("RegNo")).toString();
				if(items_processed % 100 == 0) {
					//Log.info(items_count, obj.RegNo);
					fwk->showProgress(reg, items_processed, items_count);
				}

				q.bindValue(":firstName", obj.value(QStringLiteral("FirstName")).toString());
				q.bindValue(":lastName", obj.value(QStringLiteral("LastName")).toString());
				//var name_search_key = File.toAscii7(obj.LastName + " " + obj.FirstName, true);
				//q.bindValue(":nameSearchKey", name_search_key);
				if(!reg.isEmpty()) {
					q.bindValue(":registration", reg);
					q.bindValue(":clubAbbr", reg.mid(0, 3));
				}
				q.bindValue(":licence", obj.value(QStringLiteral("Lic")).toString());
				q.bindValue(":siId", obj.value(QStringLiteral("SI")).toString().toInt());
				q.bindValue(":importId", obj.value(QStringLiteral("UserID")).toString().toInt());

				//q.bindValue(":abbr", obj.Abbr);
				//q.bindValue(":name", obj.Name);
				//q.bindValue(":importId", obj.ID);
				q.exec(qf::core::Exception::Throw);

				items_processed++;
			}
			transaction.commit();
			fwk->hideProgress();
			QMetaObject::invokeMethod(fwk->plugin("Event"), "emitDbEvent", Qt::QueuedConnection
									  , Q_ARG(QString, Event::EventPlugin::DBEVENT_REGISTRATIONS_IMPORTED)
									  , Q_ARG(QVariant, QVariant())
									  , Q_ARG(bool, true)
									  );
		}
		catch (qf::core::Exception &e) {
			qf::qmlwidgets::dialogs::MessageBox::showException(fwk, e);
		}
	});
}

void OrisImporter::importClubs()
{
	QUrl url("https://oris.orientacnisporty.cz/API/?format=json&method=getCSOSClubList");
	getJsonAndProcess(url, this, [](const QJsonDocument &jsd) {
		saveJsonBackup("Clubs", jsd);
		qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
		QJsonObject data = jsd.object().value(QStringLiteral("Data")).toObject();;
		// import clubs
		int items_processed = 0;
		int items_count = 0;
		for(auto it = data.constBegin(); it != data.constEnd(); ++it) {
			items_count++;
		}
		fwk->showProgress(tr("Importing clubs"), 1, items_count);
		try {
			qf::core::sql::Transaction transaction;
			qf::core::sql::Query q;
			q.exec("DELETE FROM clubs WHERE importId IS NOT NULL", qf::core::Exception::Throw);
			q.prepare("INSERT INTO clubs (name, abbr, importId) VALUES (:name, :abbr, :importId)", qf::core::Exception::Throw);
			for(auto it = data.constBegin(); it != data.constEnd(); ++it) {
				QJsonObject obj = it.value().toObject();
				//Log.debug(JSON.stringify(obj, null, 2));
				QString abbr = obj.value(QStringLiteral("Abbr")).toString();
				QString name = obj.value(QStringLiteral("Name")).toString();
				fwk->showProgress(abbr + ' ' + name, items_processed, items_count);
				q.bindValue(":abbr", abbr);
				q.bindValue(":name", name);
				q.bindValue(":importId", obj.value(QStringLiteral("ID")).toString().toInt());
				q.exec(qf::core::Exception::Throw);

				items_processed++;
			}
			transaction.commit();
			fwk->hideProgress();
		}
		catch (qf::core::Exception &e) {
			qf::qmlwidgets::dialogs::MessageBox::showException(fwk, e);
		}
	});
}


