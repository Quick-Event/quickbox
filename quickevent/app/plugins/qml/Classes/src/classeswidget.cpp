#include "classeswidget.h"
#include "ui_classeswidget.h"

#include "classesplugin.h"
#include "coursedef.h"

#include <EventPlugin/eventplugin.h>

#include <qf/core/string.h>
#include <qf/core/utils.h>

#include <qf/qmlwidgets/action.h>
#include <qf/qmlwidgets/menubar.h>
#include <qf/qmlwidgets/framework/mainwindow.h>
#include <qf/qmlwidgets/dialogs/filedialog.h>
#include <qf/qmlwidgets/dialogs/messagebox.h>

namespace qfc = qf::core;
namespace qfw = qf::qmlwidgets;
namespace qfd = qf::qmlwidgets::dialogs;

ClassesWidget::ClassesWidget(QWidget *parent) :
	Super(parent),
	ui(new Ui::ClassesWidget)
{
	ui->setupUi(this);
}

ClassesWidget::~ClassesWidget()
{
	delete ui;
}

void ClassesWidget::settleDownInPartWidget(ThisPartWidget *part_widget)
{
	connect(part_widget, SIGNAL(reloadRequest()), this, SLOT(reload()));

	qfw::Action *a_import = part_widget->menuBar()->actionForPath("import", true);
	a_import->setText("&Import");

	qfw::Action *a_ocad = new qfw::Action("OCad", this);
	connect(a_ocad, SIGNAL(triggered()), this, SLOT(import_ocad()));

	a_import->addActionInto(a_ocad);
}

void ClassesWidget::import_ocad()
{
	QString fn = qfd::FileDialog::getOpenFileName(this, tr("Open file"));
	if(fn.isEmpty())
		return;
	QFile f(fn);
	if(f.open(QFile::ReadOnly)) {
		QVariantList courses;
		QStringList lines;
		while (true) {
			QByteArray ba = f.readLine();
			if(ba.isEmpty())
				break;
			lines << QString::fromUtf8(ba).trimmed();
		}
		for(QString line : lines) {
			// [course];classname [,classname];0;lenght_km;climb;S1;dist_1;code_1[;dist_n;code_n];dist_finish;F1
			if(line.isEmpty())
				continue;
			CourseDef cd;
			QString course = line.section(';', 0, 0);
			qfc::String classes = line.section(';', 1, 1);
			cd.setClasses(classes.splitAndTrim(','));
			if(course.isEmpty())
				course = cd.classes().value(0);
			cd.setCourse(course);
			{
				QString s = line.section(';', 3, 3).trimmed();
				s.replace('.', QString()).replace(',', QString());
				cd.setLenght(s.toInt());
			}
			{
				QString s = line.section(';', 4, 4).trimmed();
				s.replace('.', QString()).replace(',', QString());
				cd.setClimb(s.toInt());
			}
			{
				qfc::String s = line.section(';', 6);
				QVariantList codes;
				QStringList sl = s.splitAndTrim(';');
				for (int i = 0; i < sl.count()-2; i+=2) {
					codes << sl[i+1].toInt();
				}
				cd.setCodes(codes);
			}
			courses << cd;
		}

		qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
		EventPlugin *event_plugin = qobject_cast<EventPlugin *>(fwk->plugin("Event"));
		ClassesPlugin *classes_plugin = qobject_cast<ClassesPlugin *>(fwk->plugin("Classes"));
		if(event_plugin && classes_plugin) {
			QString msg = tr("Delete all courses definitions for stage %1?").arg(event_plugin->currentStage());
			if(qfd::MessageBox::askYesNo(fwk, msg, false)) {
				classes_plugin->createCourses(event_plugin->currentStage(), courses);
				reload();
			}
		}

	}
}

void ClassesWidget::reload()
{

}
