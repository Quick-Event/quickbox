#include "orisimporter.h"

#include <Event/eventplugin.h>
#include <Classes/classesplugin.h>
#include <Classes/classdocument.h>
#include <Competitors/competitorsplugin.h>
#include <Competitors/competitordocument.h>

#include <qf/qmlwidgets/framework/mainwindow.h>
#include <qf/qmlwidgets/dialogs/getiteminputdialog.h>
#include <qf/qmlwidgets/dialogs/messagebox.h>
#include <qf/qmlwidgets/framework/plugin.h>

#include <qf/core/network/networkaccessmanager.h>
#include <qf/core/network/networkreply.h>
#include <qf/core/log.h>
#include <qf/core/sql/query.h>
#include <qf/core/sql/transaction.h>

#include <QDate>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>
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

void OrisImporter::getJsonAndProcess(const QUrl &url, std::function<void (const QJsonDocument &)> process_call_back)
{
	auto *manager = networkAccessManager();
	qf::core::network::NetworkReply *reply = manager->get(url);
	qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
	connect(reply, &qf::core::network::NetworkReply::downloadProgress, fwk, &qf::qmlwidgets::framework::MainWindow::showProgress);
	connect(reply, &qf::core::network::NetworkReply::finished, [reply, process_call_back](bool get_ok) {
		qfInfo() << "Get:" << reply->url().toString() << "OK:" << get_ok;
		qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
		if(get_ok) {
			QJsonParseError err;
			QJsonDocument jsd = QJsonDocument::fromJson(reply->data(), &err);
			if(err.error != QJsonParseError::NoError) {
				qf::qmlwidgets::dialogs::MessageBox::showError(fwk, tr("JSON document parse error: %1").arg(err.errorString()));
				return;
			}
			process_call_back(jsd);
		}
		else {
			qf::qmlwidgets::dialogs::MessageBox::showError(fwk, "http get error on: " + reply->url().toString());
		}
		reply->deleteLater();
	});
}

void OrisImporter::chooseAndImport()
{
	qfLogFuncFrame();
	QDate d = QDate::currentDate();
	d = d.addMonths(-1);
	QUrl url("http://oris.orientacnisporty.cz/API/?format=json&method=getEventList&sport=1&datefrom=" + d.toString(Qt::ISODate));
	/*
	qfInfo() << url;
	// manager cannot be an automatic value, it destroys all replies when destructed
	QNetworkAccessManager *manager = new QNetworkAccessManager(this);
	QNetworkReply *reply = manager->get(QNetworkRequest(url));
	//reply->setParent(nullptr);
	connect(reply, &QNetworkReply::finished, [reply]() {
		qfInfo() << "finished";
	});
	connect(reply, &QObject::destroyed, []() {
		qfInfo() << "destroyed";
	});
	connect(reply, &QNetworkReply::readyRead, [reply]() {
		qfInfo() << reply->readAll();
	});
	connect(reply, &QNetworkReply::downloadProgress, [](qint64 progress, qint64 total) {
		qfInfo() << progress << "/" << total;
	});
	*/
	getJsonAndProcess(url, [this](const QJsonDocument &jsd) {
		//qfWarning().noquote() << QString::fromUtf8(jsd.toJson());
		QJsonObject jso = jsd.object().value(QStringLiteral("Data")).toObject();;
		QStringList event_descriptions;
		QList<int> event_ids;
		for(auto it = jso.constBegin(); it != jso.constEnd(); ++it) {
			QJsonObject event = it.value().toObject();
			//Log.info(event.Name)
			event_ids << event.value(QStringLiteral("ID")).toString().toInt();
			QJsonObject org1 = event.value(QStringLiteral("Org1")).toObject();
			QString event_description = event.value(QStringLiteral("Date")).toString()
										+ " " + org1.value(QStringLiteral("Abbr")).toString()
										+ " " + event.value(QStringLiteral("Name")).toString();
			event_descriptions << event_description;
		}
		qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
		int ix = qf::qmlwidgets::dialogs::GetItemInputDialog::getItem(fwk, tr("Get item"), tr("Select event to import"), event_descriptions);
		if(ix >= 0) {
			qfInfo() << "importEvent:" << event_ids[ix] << event_descriptions[ix];
			importEvent(event_ids[ix]);
		}
	});
}

static QString jsonObjectToFullName(const QJsonObject &data, const QString &field_name)
{
	QJsonObject jso = data.value(field_name).toObject();
	return jso.value(QStringLiteral("FirstName")).toString() + ' ' + jso.value(QStringLiteral("LastName")).toString();
}

void OrisImporter::importEvent(int event_id)
{
	QUrl url(QString("http://oris.orientacnisporty.cz/API/?format=json&method=getEvent&id=%1").arg(event_id));
	getJsonAndProcess(url, [this, event_id](const QJsonDocument &jsd) {
		qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
		try {
			qf::core::sql::Transaction transaction;

			QJsonObject data = jsd.object().value(QStringLiteral("Data")).toObject();;
			int stage_count = data.value(QStringLiteral("Stages")).toString().toInt();
			if(!stage_count)
				stage_count = 1;
			qfInfo() << "pocet etap:" << stage_count;
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
			ecfg["importId"] = event_id;
			if(!eventPlugin()->createEvent(QString(), ecfg))
				return;
			//QString event_name = eventPlugin()->eventName();

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
		importEventOrisRunners(event_id);
	});
}

void OrisImporter::importEventOrisRunners(int event_id)
{
	QUrl url(QString("http://oris.orientacnisporty.cz/API/?format=json&method=getEventEntries&eventid=%1").arg(event_id));
	getJsonAndProcess(url, [](const QJsonDocument &jsd) {
		qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
		try {
			qf::core::sql::Transaction transaction;
			QJsonObject data = jsd.object().value(QStringLiteral("Data")).toObject();;
			int items_processed = 0;
			int items_count = 0;
			for(auto it = data.constBegin(); it != data.constEnd(); ++it) {
				items_count++;
			}
			QSet<int> siids_imported;
			Competitors::CompetitorDocument doc;
			for(auto it = data.constBegin(); it != data.constEnd(); ++it) {
				QJsonObject competitor_o = it.value().toObject();
				doc.loadForInsert();
				QString siid_str = competitor_o.value(QStringLiteral("SI")).toString();
				bool ok;
				int siid = siid_str.toInt(&ok);
				//qfInfo() << "SI:" << siid, competitor_obj.ClassDesc, ' ', competitor_obj.LastName, ' ', competitor_obj.FirstName, "classId:", parseInt(competitor_obj.ClassID));
				QString note = competitor_o.value(QStringLiteral("Note")).toString();
				if(!ok) {
					note += " SI:" + siid_str;
				}
				bool siid_duplicit = false;
				if(siid > 0) {
					// check duplicit SI
					siid_duplicit = siids_imported.contains(siid);
					if(!siid_duplicit)
						siids_imported << siid;
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
				if(siid_duplicit)
					qfWarning() << tr("%1 %2 %3 SI: %4 is duplicit!").arg(reg_no).arg(last_name).arg(first_name).arg(siid);
				doc.loadForInsert();
				doc.setValue("classId", competitor_o.value(QStringLiteral("ClassID")).toString().toInt());
				doc.setValue("siId", siid);
				doc.setValue("firstName", first_name);
				doc.setValue("lastName", last_name);
				doc.setValue("registration", reg_no);
				doc.setValue("licence", competitor_o.value(QStringLiteral("Licence")).toString());
				doc.setValue("note", note);
				doc.setValue("importId", competitor_o.value(QStringLiteral("ID")).toString().toInt());
				doc.setSaveSiidToRuns(!siid_duplicit);
				doc.save();
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

void OrisImporter::importRegistrations()
{
	int year = QDate::currentDate().year();
	QUrl url(QString("http://oris.orientacnisporty.cz/API/?format=json&method=getRegistration&sport=1&year=%1").arg(year));
	getJsonAndProcess(url, [](const QJsonDocument &jsd) {
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
				//Log.debug(JSON.stringify(obj, null, 2));
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
				q.bindValue(":siId", obj.value(QStringLiteral("SI")).toString());
				q.bindValue(":importId", obj.value(QStringLiteral("UserID")).toString());

				//q.bindValue(":abbr", obj.Abbr);
				//q.bindValue(":name", obj.Name);
				//q.bindValue(":importId", obj.ID);
				q.exec(qf::core::Exception::Throw);

				items_processed++;
			}
			transaction.commit();
			fwk->hideProgress();
			//fwk->plugin("Event").emitDbEvent("Oris.registrationImported", null, true);
			QMetaObject::invokeMethod(fwk->plugin("Event"), "emitDbEvent", Qt::QueuedConnection
									  , Q_ARG(QString, "Oris.registrationImported")
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
	QUrl url("http://oris.orientacnisporty.cz/API/?format=json&method=getCSOSClubList");
	getJsonAndProcess(url, [](const QJsonDocument &jsd) {
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


