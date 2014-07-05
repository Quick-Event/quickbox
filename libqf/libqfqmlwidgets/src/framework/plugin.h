#ifndef QF_QMLWIDGETS_FRAMEWORK_PLUGIN_H
#define QF_QMLWIDGETS_FRAMEWORK_PLUGIN_H

#include "../qmlwidgetsglobal.h"

#include <QObject>

namespace qf {
namespace qmlwidgets {
namespace framework {

class PluginManifest;

class QFQMLWIDGETS_DECL_EXPORT Plugin : public QObject
{
	Q_OBJECT
	Q_PROPERTY(qf::qmlwidgets::framework::PluginManifest* manifest READ manifest)
public:
	explicit Plugin(QObject *parent = 0);
	~Plugin() Q_DECL_OVERRIDE;
public:
	PluginManifest* manifest() const { return m_manifest; }
	void setManifest(PluginManifest *m);
private:
	PluginManifest* m_manifest;
};

}}}

#endif
