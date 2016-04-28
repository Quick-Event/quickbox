#include "chooseoriseventdialog.h"
#include "ui_chooseoriseventdialog.h"

#include "orisimporter.h"

#include <qf/core/utils.h>

#include <QDate>
#include <QUrl>
#include <QJsonObject>
#include <QJsonDocument>
#include <QCompleter>

ChooseOrisEventDialog::ChooseOrisEventDialog(OrisImporter *importer, QWidget *parent)
	: QDialog(parent)
	, ui(new Ui::ChooseOrisEventDialog)
	, m_importer(importer)
{
	ui->setupUi(this);
}

ChooseOrisEventDialog::~ChooseOrisEventDialog()
{
	delete ui;
}

int ChooseOrisEventDialog::exec()
{
	load();
	return Super::exec();
}

int ChooseOrisEventDialog::eventId()
{
	return ui->edEventId->value();
}

void ChooseOrisEventDialog::load()
{
	QDate d = QDate::currentDate();
	d = d.addMonths(-1);
	QUrl url("http://oris.orientacnisporty.cz/API/?format=json&method=getEventList&all=1&datefrom=" + d.toString(Qt::ISODate));
	m_importer->getJsonAndProcess(url, [this](const QJsonDocument &jsd) {
		//qfWarning().noquote() << QString::fromUtf8(jsd.toJson());
		OrisImporter::saveJsonBackup("EventList", jsd);
		QJsonObject jso = jsd.object().value(QStringLiteral("Data")).toObject();
		QMap<QString, int> events_by_descr; //event_descr->event_id
		for(auto it = jso.constBegin(); it != jso.constEnd(); ++it) {
			QJsonObject event = it.value().toObject();
			int event_id = event.value(QStringLiteral("ID")).toString().toInt();
			QJsonObject org1 = event.value(QStringLiteral("Org1")).toObject();
			QJsonObject sport = event.value(QStringLiteral("Sport")).toObject();
			int sport_id = sport.value(QStringLiteral("ID")).toString().toInt();
			QString sport_abbr = (sport_id == 1)? tr("OB"): (sport_id == 2)? tr("LOB"): (sport_id == 3)? tr("MTBO"): tr("???");
			QString event_description = event.value(QStringLiteral("Date")).toString()
										+ " " + QString::number(event_id)
										+ " " + sport_abbr
										+ " " + org1.value(QStringLiteral("Abbr")).toString()
										+ " " + event.value(QStringLiteral("Name")).toString();
			//qfInfo() << event_description;
			ui->cbxOrisEvent->addItem(event_description, event_id);
			events_by_descr[event_description] = event_id;
		}
		QF_SAFE_DELETE(m_completer);
		m_completer = new QCompleter(events_by_descr.keys(), this);
		m_completer->setFilterMode(Qt::MatchContains);
		//ui->edOrisEvent->setCompleter(m_completer);
		ui->cbxOrisEvent->setCompleter(m_completer);
	});
}

void ChooseOrisEventDialog::on_cbxOrisEvent_currentIndexChanged(int ix)
{
	int event_id = ui->cbxOrisEvent->itemData(ix).toInt();
	ui->edEventId->setValue(event_id);
}
