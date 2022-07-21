#ifndef CLASSES_CLASSESPLUGIN_H
#define CLASSES_CLASSESPLUGIN_H

#include <qf/qmlwidgets/framework/plugin.h>

#include <qf/core/utils.h>

namespace qf {
namespace qmlwidgets {
class Action;
namespace framework { class PartWidget; }
}
}

namespace quickevent { namespace core { class CodeDef; }}

class ImportCourseDef;

namespace Classes {

class ClassesPlugin : public qf::qmlwidgets::framework::Plugin
{
	Q_OBJECT
	Q_PROPERTY(qf::qmlwidgets::framework::PartWidget* partWidget READ partWidget FINAL)
private:
	typedef qf::qmlwidgets::framework::Plugin Super;
public:
	ClassesPlugin(QObject *parent = nullptr);

	qf::qmlwidgets::framework::PartWidget *partWidget() {return m_partWidget;}

	QObject* createClassDocument(QObject *parent);
	void createClass(const QString &class_name);
	void dropClass(int class_id);
	void createCourses(int stage_id, const QList<ImportCourseDef> &courses, const QList<quickevent::core::CodeDef> &codes, bool delete_current);
	void deleteCourses(int stage_id);
	void gcCourses();

	Q_SLOT void onInstalled();
	Q_SIGNAL void nativeInstalled();
private:
	qf::qmlwidgets::framework::PartWidget *m_partWidget = nullptr;
};

}

#endif // CLASSESPLUGIN_H
