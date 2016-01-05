#include "orisimporter.h"

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

void OrisImporter::getJsonToProcess(const QUrl &url, std::function<void (const QJsonDocument &)> process_call_back)
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
	getJsonToProcess(url, [](const QJsonDocument &jsd) {
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
		}
	});
}

void OrisImporter::importRegistrations()
{
	int year = QDate::currentDate().year();
	QUrl url(QString("http://oris.orientacnisporty.cz/API/?format=json&method=getRegistration&sport=1&year=%1").arg(year));
	getJsonToProcess(url, [](const QJsonDocument &jsd) {
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


