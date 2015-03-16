#ifndef CLASSESPLUGIN_H
#define CLASSESPLUGIN_H

#include <qf/qmlwidgets/framework/plugin.h>

#include <qf/core/utils.h>

namespace qf {
namespace qmlwidgets {
class Action;
namespace framework {
class PartWidget;
}
}
}
class ClassesPlugin : public qf::qmlwidgets::framework::Plugin
{
	Q_OBJECT
	Q_PROPERTY(qf::qmlwidgets::framework::PartWidget* partWidget READ partWidget FINAL)
private:
	typedef qf::qmlwidgets::framework::Plugin Super;
public:
	ClassesPlugin(QObject *parent = nullptr);

	qf::qmlwidgets::framework::PartWidget *partWidget() {return m_partWidget;}

	Q_INVOKABLE QObject* createClassDocument(QObject *parent);
	Q_INVOKABLE void createCourses(int current_stage, const QVariantList &courses);

	Q_SIGNAL void nativeInstalled();
private:
	Q_SLOT void onInstalled();
private:
	qf::qmlwidgets::framework::PartWidget *m_partWidget = nullptr;
};

#endif // CLASSESPLUGIN_H
