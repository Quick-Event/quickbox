#include "txtimporter.h"

#include <Event/eventplugin.h>
#include <Competitors/competitordocument.h>

#include <qf/qmlwidgets/framework/mainwindow.h>
#include <qf/qmlwidgets/dialogs/filedialog.h>
#include <qf/qmlwidgets/dialogs/messagebox.h>

#include <qf/core/assert.h>
#include <qf/core/exception.h>
#include <qf/core/sql/query.h>
#include <qf/core/sql/transaction.h>
#include <qf/core/utils/csvreader.h>

#include <QTextStream>

namespace qfc = qf::core;
namespace qfw = qf::qmlwidgets;
namespace qfd = qf::qmlwidgets::dialogs;
namespace qfm = qf::core::model;
namespace qfs = qf::core::sql;

static Event::EventPlugin* eventPlugin()
{
	qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
	auto *plugin = qobject_cast<Event::EventPlugin*>(fwk->plugin("Event"));
	QF_ASSERT_EX(plugin != nullptr, "Bad event plugin!");
	return plugin;
}

TxtImporter::TxtImporter(QObject *parent)
	: QObject(parent)
{
}

void TxtImporter::importCompetitorsCSOS()
{
	qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
	qf::qmlwidgets::dialogs::MessageBox mbx(fwk);
	mbx.setIcon(QMessageBox::Information);
	mbx.setText(tr("Import windows-1250 coded fixed column size text files in CSOS format."));
	mbx.setInformativeText(tr("Each row should have following columns: "
							  "<ol>"
							  "<li>7 chars: Registration</li>"
							  "<li>1 space</li>"
							  "<li>10 chars: Class</li>"
							  "<li>1 space</li>"
							  "<li>10 chars: SI</li>"
							  "<li>1 space</li>"
							  "<li>25 chars: Name</li>"
							  "<li>1 space</li>"
							  "<li>2 chars: Licence</li>"
							  "<li>1 space</li>"
							  "<li>rest of line: Note</li>"
							  "</ol>"));
	mbx.setDoNotShowAgainPersistentKey("importCompetitorsCSOS");
	int res = mbx.exec();
	//qfInfo() << "RES:" << res;
	if(res != QMessageBox::Ok)
		return;
	QString fn = qfd::FileDialog::getOpenFileName(fwk, tr("Open file"), QString(), tr("CSOS files (*.txt)"));
	if(fn.isEmpty())
		return;
	try {
		QFile f(fn);
		if(!f.open(QFile::ReadOnly))
			QF_EXCEPTION(tr("Cannot open file '%1' for reading.").arg(fn));
		QList<QVariantList> csv_rows;
		QTextStream stream(&f);
		stream.setCodec("windows-1250");
		QString line;
		while (stream.readLineInto(&line)) {
			if(line.isEmpty())
				continue;
			if(line.startsWith('#'))
				continue;
			// reg 7, ' ', class 10, ' ', SI 10, ' ', 25 name, ' ', 2 lic, ' ', note
			QString reg = line.mid(0, 7).trimmed();
			QString class_name = line.mid(8, 10).trimmed();
			int si = line.mid(19, 10).toInt();
			QString last_name = line.mid(30, 25).trimmed();
			QString lic = line.mid(56, 2).trimmed();
			QString note = line.mid(59).trimmed();
			csv_rows << (QVariantList() << reg << class_name << si << last_name << QString() << lic << note);
		}
		qf::core::sql::Transaction transaction;
		importParsedCsv(csv_rows);
		transaction.commit();
		emit eventPlugin()->reloadDataRequest();
	}
	catch (qf::core::Exception &e) {
		qf::qmlwidgets::dialogs::MessageBox::showException(fwk, e);
	}
}

void TxtImporter::importCompetitorsCSV()
{
	qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
	qf::qmlwidgets::dialogs::MessageBox mbx(fwk);
	mbx.setIcon(QMessageBox::Information);
	mbx.setText(tr("Import comma separated values UTF8 text files with header."));
	mbx.setInformativeText(tr("Each row should have following columns: "
							  "<ol>"
							  "<li>Registration</li>"
							  "<li>Class</li>"
							  "<li>SI</li>"
							  "<li>LastName</li>"
							  "<li>FirstName</li>"
							  "<li>Licence</li>"
							  "<li>Note</li>"
							  "</ol>"));
	mbx.setDoNotShowAgainPersistentKey("importCompetitorsCSV");
	int res = mbx.exec();
	if(res != QMessageBox::Ok)
		return;
	QString fn = qfd::FileDialog::getOpenFileName(fwk, tr("Open file"), QString(), tr("CSV files (*.csv *.txt)"));
	if(fn.isEmpty())
		return;
	try {
		QFile f(fn);
		if(!f.open(QFile::ReadOnly))
			QF_EXCEPTION(tr("Cannot open file '%1' for reading.").arg(fn));
		QList<QVariantList> csv_rows;
		QTextStream ts(&f);
		ts.setCodec("utf-8");
		qf::core::utils::CSVReader reader(&ts);
		reader.setSeparator(',');
		reader.setLineComment('#');
		/// skip header
		if (!ts.atEnd())
			reader.readCSVLine();
		while (!ts.atEnd()) {
			QStringList sl = reader.readCSVLineSplitted();
			QString reg = sl.value(ColRegistration).trimmed();
			QString class_name = sl.value(ColClassName).trimmed();
			int si = sl.value(ColSiId).trimmed().toInt();
			QString first_name = sl.value(ColFirstName).trimmed();
			QString last_name = sl.value(ColLastName).trimmed();
			QString lic = sl.value(ColLicence).trimmed();
			QString note = sl.value(ColNote).trimmed();
			csv_rows << (QVariantList() << reg << class_name << si << last_name << first_name << lic << note);
		}
		qf::core::sql::Transaction transaction;
		importParsedCsv(csv_rows);
		transaction.commit();
		emit eventPlugin()->reloadDataRequest();
	}
	catch (qf::core::Exception &e) {
		qf::qmlwidgets::dialogs::MessageBox::showException(fwk, e);
	}
}

void TxtImporter::importRankingCsv()
{
	qfLogFuncFrame();
	qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
	QString fn = qfd::FileDialog::getOpenFileName(fwk, tr("Open file"), QString(), tr("Oris ranking CSV files (*.txt *.csv)"));
	if(fn.isEmpty())
		return;
	try {
		QFile f(fn);
		if(!f.open(QFile::ReadOnly))
			QF_EXCEPTION(tr("Cannot open file '%1' for reading.").arg(fn));
		QTextStream ts(&f);
		qf::core::utils::CSVReader reader(&ts);
		reader.setSeparator(';');
		//reader.setLineComment('#');
		enum {ColPos = 0, ColLastName, ColFirstName, ColRegistration, ColPoints, ColCoef};

		qf::core::sql::Transaction transaction;
		qf::core::sql::Query q;
		q.prepare("UPDATE competitors SET ranking=:ranking WHERE registration=:registration", qf::core::Exception::Throw);

		int n = 0;
		while (!ts.atEnd()) {
			QStringList line = reader.readCSVLineSplitted();
			if(line.count() <= 1)
				QF_EXCEPTION(tr("Fields separation error, invalid CSV format, Error reading CSV line: [%1]").arg(line.join(';').mid(0, 100)));
			if(n++ == 0) // skip column names
				continue;
			QString registration = line.value(ColRegistration);
			int pos = line.value(ColPos).toInt();
			if(pos == 0 || registration.isEmpty()) {
				QF_EXCEPTION(tr("Error reading CSV line: [%1]").arg(line.join(';')));
			}
			qfDebug() << registration << "->" << pos;
			q.bindValue(":ranking", pos);
			q.bindValue(":registration", registration);
			q.exec(qf::core::Exception::Throw);
		}
		transaction.commit();
		qfInfo() << fn << n << "lines imported";
	}
	catch (const qf::core::Exception &e) {
		qf::qmlwidgets::dialogs::MessageBox::showException(fwk, e);
	}
}

void TxtImporter::importParsedCsv(const QList<QVariantList> &csv)
{
	QMap<QString, int> classes_map; // classes.name->classes.id
	qf::core::sql::Query q;
	q.exec("SELECT id, name FROM classes", qf::core::Exception::Throw);
	while(q.next()) {
		classes_map[q.value(1).toString()] = q.value(0).toInt();
	}
	QSet<int> used_idsi;
	for(const QVariantList &row : csv) {
		Competitors::CompetitorDocument doc;
		doc.loadForInsert();
		int siid = row.value(ColSiId).toInt();
		//qfInfo() << "SI:" << siid, competitor_obj.ClassDesc, ' ', competitor_obj.LastName, ' ', competitor_obj.FirstName, "classId:", parseInt(competitor_obj.ClassID));
		QString note = row.value(ColNote).toString();
		QString first_name = row.value(ColFirstName).toString();
		QString last_name = row.value(ColLastName).toString();
		if(first_name.isEmpty() && !last_name.isEmpty()) {
			int ix = last_name.indexOf(' ');
			if(ix > 0) {
				first_name = last_name.mid(ix + 1).trimmed();
				last_name = last_name.mid(0, ix).trimmed();
			}
		}
		QString reg = row.value(ColRegistration).toString();
		QString class_name = row.value(ColClassName).toString();
		int class_id = classes_map.value(class_name);
		if(class_id == 0)
			QF_EXCEPTION(tr("Undefined class name: '%1'").arg(class_name));
		//fwk->showProgress("Importing: " + reg_no + ' ' + last_name + ' ' + first_name, items_processed, items_count);
		//	qfWarning() << tr("%1 %2 %3 SI: %4 is duplicit!").arg(reg_no).arg(last_name).arg(first_name).arg(siid);
		doc.setValue("classId", class_id);
		if(siid > 0) {
			bool is_unique = !used_idsi.contains(siid);
			if(is_unique)
				used_idsi << siid;
			doc.setSiid(siid, is_unique);
		}
		doc.setValue("firstName", first_name);
		doc.setValue("lastName", last_name);
		doc.setValue("registration", reg);
		doc.setValue("licence", row.value(ColLicence).toString());
		doc.setValue("note", note);
		doc.save();
	}

}
