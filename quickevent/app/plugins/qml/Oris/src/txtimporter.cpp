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

void TxtImporter::importParsedCsv(const QList<QVariantList> &csv)
{
	QMap<QString, int> classes_map; // classes.name->classes.id
	qf::core::sql::Query q;
	q.exec("SELECT id, name FROM classes", qf::core::Exception::Throw);
	while(q.next()) {
		classes_map[q.value(1).toString()] = q.value(0).toInt();
	}
	//QSet<int> used_idsi;
	for(const QVariantList &row : csv) {
		Competitors::CompetitorDocument doc;
		//doc.setSaveSiidToRuns(true);
		doc.loadForInsert();
		int siid = row.value(ColSiId).toInt();
		//qfInfo() << "SI:" << siid, competitor_obj.ClassDesc, ' ', competitor_obj.LastName, ' ', competitor_obj.FirstName, "classId:", parseInt(competitor_obj.ClassID));
		QString note = row.value(ColNote).toString();
		QString first_name = row.value(ColFirstName).toString();
		QString last_name = row.value(ColLastName).toString();
		if(first_name.isEmpty() && !last_name.isEmpty()) {
			int ix = last_name.indexOf(' ');
			if(ix > 0) {
				first_name = last_name.mid(0, ix).trimmed();
				last_name = last_name.mid(ix + 1).trimmed();
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
		if(siid > 0)
			doc.setValue("siId", siid);
		doc.setValue("firstName", first_name);
		doc.setValue("lastName", last_name);
		doc.setValue("registration", reg);
		doc.setValue("licence", row.value(ColLicence).toString());
		doc.setValue("note", note);
		doc.save();
	}

}
