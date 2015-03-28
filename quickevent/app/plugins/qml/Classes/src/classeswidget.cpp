#include "classeswidget.h"
#include "ui_classeswidget.h"

#include "classesplugin.h"
#include "coursedef.h"

#include <EventPlugin/eventplugin.h>

#include <qf/core/string.h>
#include <qf/core/utils.h>
#include <qf/core/collator.h>

#include <qf/core/model/sqltablemodel.h>
#include <qf/core/sql/querybuilder.h>

#include <qf/qmlwidgets/action.h>
#include <qf/qmlwidgets/menubar.h>
#include <qf/qmlwidgets/framework/mainwindow.h>
#include <qf/qmlwidgets/dialogs/filedialog.h>
#include <qf/qmlwidgets/dialogs/messagebox.h>

namespace qfc = qf::core;
namespace qfw = qf::qmlwidgets;
namespace qfd = qf::qmlwidgets::dialogs;
namespace qfm = qf::core::model;
namespace qfs = qf::core::sql;

ClassesWidget::ClassesWidget(QWidget *parent) :
	Super(parent),
	ui(new Ui::ClassesWidget)
{
	ui->setupUi(this);
	{
		ui->tblClassesTB->setTableView(ui->tblClasses);
		qfm::SqlTableModel *m = new qfm::SqlTableModel(this);
		m->addColumn("id").setReadOnly(true);
		m->addColumn("classes.name", tr("Class"));
		m->addColumn("classdefs.startTimeMin", tr("Start"));
		m->addColumn("classdefs.startIntervalMin", tr("Interval"));
		m->addColumn("classdefs.vacantsBefore", tr("VB")).setToolTip(tr("Vacants before"));
		m->addColumn("classdefs.vacantEvery", tr("VE")).setToolTip(tr("Vacant every"));
		m->addColumn("classdefs.vacantsAfter", tr("VA")).setToolTip(tr("Vacants after"));
		m->addColumn("classdefs.lastTimeMin", tr("Last"));
		m->addColumn("classdefs.mapCount", tr("Maps"));
		m->addColumn("courses.name", tr("Course"));
		m->addColumn("courses.length", tr("Length"));
		m->addColumn("courses.climb", tr("Climb"));
		ui->tblClasses->setTableModel(m);
		m_classesModel = m;
	}
	{
		ui->tblCourseCodesTB->setTableView(ui->tblCourseCodes);
		qfm::SqlTableModel *m = new qfm::SqlTableModel(this);
		m->addColumn("coursecodes.position", tr("Pos")).setReadOnly(true);
		m->addColumn("codes.code", tr("Code")).setReadOnly(true);
		m->addColumn("codes.broken", tr("Broken")).setReadOnly(true);
		ui->tblCourseCodes->setTableModel(m);
		m_courseCodesModel = m;
	}
	connect(ui->tblClasses, SIGNAL(currentRowChanged(int)), this, SLOT(reloadCourseCodes()));
}

ClassesWidget::~ClassesWidget()
{
	delete ui;
}

void ClassesWidget::settleDownInPartWidget(ThisPartWidget *part_widget)
{
	connect(part_widget, SIGNAL(resetPartRequest()), this, SLOT(reset()));
	connect(part_widget, SIGNAL(reloadPartRequest()), this, SLOT(reload()));

	qfw::Action *a_import = part_widget->menuBar()->actionForPath("import", true);
	a_import->setText("&Import");

	qfw::Action *a_ocad = new qfw::Action("OCad", this);
	connect(a_ocad, SIGNAL(triggered()), this, SLOT(import_ocad()));

	a_import->addActionInto(a_ocad);
}

void ClassesWidget::reset()
{
	reload();
}

void ClassesWidget::reload()
{
	qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
	EventPlugin *event_plugin = qobject_cast<EventPlugin *>(fwk->plugin("Event"));
	{
		qfs::QueryBuilder qb;
		qb.select2("classes", "*")
				.select2("classdefs", "*")
				.select2("courses", "name, length, climb")
				.from("classes")
				.joinRestricted("classes.id", "classdefs.classId", "classdefs.stageId=" QF_IARG(event_plugin->currentStage()))
				.join("classdefs.courseId", "courses.id")
				.orderBy("classes.name");//.limit(10);
		/*
		int class_id = m_cbxClasses->currentData().toInt();
		if(class_id > 0) {
			qb.where("competitors.classId=" + QString::number(class_id));
		}
		*/
		m_classesModel->setQueryBuilder(qb);
		m_classesModel->reload();
	}
	reloadCourseCodes();
}

void ClassesWidget::reloadCourseCodes()
{
	int current_course_id = 0;
	auto row = ui->tblClasses->selectedRow();
	if(!row.isNull())
		current_course_id = row.value("classdefs.courseId").toInt();
	if(current_course_id == 0) {
		ui->lblCourseCodes->setText("---");
	}
	else {
		ui->lblCourseCodes->setText(row.value("courses.name").toString());
	}
	//qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
	//EventPlugin *event_plugin = qobject_cast<EventPlugin *>(fwk->plugin("Event"));
	{
		qfs::QueryBuilder qb;
		qb.select2("codes", "*")
				.select2("coursecodes", "position")
				.from("coursecodes")
				.join("coursecodes.codeId", "codes.id")
				.where("coursecodes.courseId=" QF_IARG(current_course_id))
				.orderBy("coursecodes.position");
		m_courseCodesModel->setQueryBuilder(qb);
		m_courseCodesModel->reload();
	}
}

static QString normalize_course_name(const QString &name)
{
	QString ret = qf::core::Collator::toAscii7(name, false);
	ret.replace(' ', QString());
	ret.replace(',', '+');
	return ret;
}

void ClassesWidget::import_ocad()
{
	QString fn = qfd::FileDialog::getOpenFileName(this, tr("Open file"));
	if(fn.isEmpty())
		return;
	QFile f(fn);
	if(f.open(QFile::ReadOnly)) {
		QStringList lines;
		while (true) {
			QByteArray ba = f.readLine();
			if(ba.isEmpty())
				break;
			lines << QString::fromUtf8(ba).trimmed();
		}
		try {
			//QSet<QString> parsed_classes;
			//QSet<QString> parsed_courses;
			QMap<QString, CourseDef> defined_courses;
			for(QString line : lines) {
				// [classname];coursename;0;lenght_km;climb;S1;dist_1;code_1[;dist_n;code_n];dist_finish;F1
				if(line.isEmpty())
					continue;
				QStringList class_names;
				qfc::String course_name = normalize_course_name(line.section(';', 1, 1));
				QString class_name = line.section(';', 0, 0);
				if(class_name.isEmpty()) {
					for(auto ch : {'-', ',', ':', '+'}) {
						if(course_name.contains(ch)) {
							class_names = course_name.splitAndTrim(ch);
							break;
						}
					}
				}
				if(class_names.isEmpty())
					class_names << course_name;
				if(defined_courses.contains(course_name)) {
					CourseDef cd = defined_courses.value(course_name);
					QStringList classes = cd.classes() << class_names;
					defined_courses[course_name].setClasses(classes);
					continue;
				}
				CourseDef &cd = defined_courses[course_name];
				cd.setCourse(course_name);
				cd.setClasses(class_names);
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
			}
			QVariantList courses;
			for(auto cd : defined_courses.values())
				courses << cd;
			{
				QJsonDocument doc = QJsonDocument::fromVariant(courses);
				qfDebug() << doc.toJson();
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
		catch (const qf::core::Exception &e) {
			qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
			qf::qmlwidgets::dialogs::MessageBox::showException(fwk, e);
		}
	}
}

