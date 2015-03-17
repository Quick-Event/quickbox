#include "classesplugin.h"
#include "thispartwidget.h"
#include "classdocument.h"

#include "coursedef.h"

#include <EventPlugin/eventplugin.h>

#include <qf/qmlwidgets/framework/mainwindow.h>
#include <qf/qmlwidgets/action.h>

#include <qf/core/log.h>
#include <qf/core/exception.h>
#include <qf/core/sql/query.h>
#include <qf/core/sql/querybuilder.h>
#include <qf/core/sql/transaction.h>

#include <QQmlEngine>

namespace qfw = qf::qmlwidgets;
namespace qff = qf::qmlwidgets::framework;
//namespace qfd = qf::qmlwidgets::dialogs;
namespace qfs = qf::core::sql;

ClassesPlugin::ClassesPlugin(QObject *parent)
	: Super(parent)
{
	connect(this, &ClassesPlugin::installed, this, &ClassesPlugin::onInstalled, Qt::QueuedConnection);
}

void ClassesPlugin::onInstalled()
{
	qff::MainWindow *fwk = qff::MainWindow::frameWork();
	m_partWidget = new ThisPartWidget();
	fwk->addPartWidget(m_partWidget, manifest()->featureId());

	emit nativeInstalled();
}

QObject *ClassesPlugin::createClassDocument(QObject *parent)
{
	ClassDocument *ret = new ClassDocument(parent);
	if(!parent) {
		qfWarning() << "Parent is NULL, created class will have QQmlEngine::JavaScriptOwnership.";
		qmlEngine()->setObjectOwnership(ret, QQmlEngine::JavaScriptOwnership);
	}
	return ret;
}

static QString join_str(const QList<int> &lst)
{
	QStringList ret;
	for(auto i : lst)
		ret << QString::number(i);
	return ret.join(',');
}

void ClassesPlugin::createCourses(int current_stage, const QVariantList &courses)
{
	qfLogFuncFrame();
	qf::core::sql::Transaction transaction(qf::core::sql::Connection::forName());
	try {
		qf::core::sql::Query q;
		{
			QList<int> courses_ids;
			{
				q.exec("SELECT courseId FROM classdefs WHERE stageId=" QF_IARG(current_stage));
				while(q.next()) {
					courses_ids << q.value("courseId").toInt();
				}
			}
			if(!courses_ids.isEmpty()) {
				q.exec("DELETE FROM codes WHERE id IN ( SELECT codeId FROM coursecodes WHERE courseId IN (" + join_str(courses_ids) + ") )", qf::core::Exception::Throw);
				q.exec("DELETE FROM coursecodes WHERE courseId IN (" + join_str(courses_ids) + ") )", qf::core::Exception::Throw);
				q.exec("DELETE FROM courses WHERE id IN (" + join_str(courses_ids) + ") )", qf::core::Exception::Throw);
			}
			q.exec("DELETE FROM classdefs WHERE stageId=" QF_IARG(current_stage), qf::core::Exception::Throw);
		}
		QSet<int> codes;
		QMap<QString, int> course_ids;
		QMap<int, int> course_codes;
		QMap<QString, int> class_course_ids;
		QMap<QString, int> class_ids;
		{
			q.exec("SELECT id, name FROM classes");
			while(q.next()) {
				class_ids[q.value("name").toString()] = q.value("id").toInt();
			}
		}
		for(auto v : courses) {
			CourseDef cd(v.toMap());
			int course_id = 0;
			{
				qDebug() << "inserting course" << cd.course();
				QString qs = "INSERT INTO courses (name, length, climb) VALUES (:name, :length, :climb)";
				q.prepare(qs, qf::core::Exception::Throw);
				q.bindValue(":name", cd.course());
				q.bindValue(":lenght", cd.lenght());
				q.bindValue(":climb", cd.climb());
				q.exec(qf::core::Exception::Throw);
				course_id = q.lastInsertId().toInt();
			}
			{
				QString qs = "INSERT INTO classdefs (classId, courseId, stageId) VALUES (:classId, :courseId, :stageId)";
				q.prepare(qs, qf::core::Exception::Throw);
				for(auto class_name : cd.classes()) {
					qDebug() << "inserting classdefs" << class_name;
					q.bindValue(":classId", class_ids.value(class_name));
					q.bindValue(":courseId", course_id);
					q.bindValue(":stageId", current_stage);
					q.exec(qf::core::Exception::Throw);
				}
			}
			course_ids[cd.course()] = course_id;
			for(auto v : cd.codes()) {
				int code = v.toInt();
				codes << code;
				course_codes[course_id] = code;
			}
		}
		{
			QString qs = "INSERT INTO codes (id) VALUES (:id)";
			q.prepare(qs, qf::core::Exception::Throw);
			for(auto code : codes) {
				qDebug() << "inserting code" << code;
				q.bindValue(":id", code);
				q.exec(qf::core::Exception::Throw);
			}
		}
		{
			QString qs = "INSERT INTO coursecodes (courseId, codeId) VALUES (:courseId, :codeId)";
			q.prepare(qs, qf::core::Exception::Throw);
			QMapIterator<int, int> it(course_codes);
			while(it.hasNext()) {
				it.next();
				qDebug() << "courseId" << it.key() << "-> codeId:" << it.value();
				q.bindValue(":courseId", it.key());
				q.bindValue(":codeId", it.value());
				q.exec(qf::core::Exception::Throw);
			}
		}
		transaction.commit();
	}
	catch (const qf::core::Exception &e) {
		qfError() << e.toString();
	}
}

