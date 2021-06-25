#ifndef QF_QMLWIDGETS_FRAMEWORK_PLUGIN_H
#define QF_QMLWIDGETS_FRAMEWORK_PLUGIN_H

#include "../qmlwidgetsglobal.h"
#include "pluginmanifest.h"

#include <qf/core/utils.h>

#include <QObject>

class QQmlEngine;

namespace qf {
namespace qmlwidgets {
namespace framework {

class QFQMLWIDGETS_DECL_EXPORT Plugin : public QObject
{
	Q_OBJECT
	Q_PROPERTY(qf::qmlwidgets::framework::PluginManifest* manifest READ manifest NOTIFY manifestChanged)
public:
	explicit Plugin(const QString &feature_id, QObject *parent = nullptr);
	explicit Plugin(QObject *parent = nullptr);
	~Plugin() Q_DECL_OVERRIDE;

	//Q_INVOKABLE QString homeDir() const;

	PluginManifest* manifest() const {return m_manifest;}
	void setManifest(PluginManifest *mf);
	Q_SIGNAL void manifestChanged(PluginManifest *mf);

	QQmlEngine* qmlEngine();

	Q_SIGNAL void installed();
private:
	PluginManifest *m_manifest = nullptr;
};

}}}

#endif
