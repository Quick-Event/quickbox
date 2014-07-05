#ifndef QF_QMLWIDGETS_FRAMEWORK_PLUGINMANIFEST_H
#define QF_QMLWIDGETS_FRAMEWORK_PLUGINMANIFEST_H

#include "../qmlwidgetsglobal.h"

#include <QObject>
#include <QStringList>

namespace qf {
namespace qmlwidgets {
namespace framework {

class QFQMLWIDGETS_DECL_EXPORT PluginManifest : public QObject
{
	Q_OBJECT
	Q_PROPERTY(QString featureId READ featureId WRITE setFeatureId FINAL)
	Q_PROPERTY(QStringList dependsOnFeatureIds READ dependsOnFeatureIds WRITE setDependsOnFeatureIds FINAL)
public:
	explicit PluginManifest(QObject *parent = 0);
	~PluginManifest() Q_DECL_OVERRIDE;
public:
	QString featureId() const { return m_featureId; }
	void setFeatureId(QString id);

	QStringList dependsOnFeatureIds() const { return m_dependsOnFeatureIds; }
	void setDependsOnFeatureIds(QStringList ids);

	QString pluginHomeDirectory() {return m_pluginHomeDirectory;}
	void setPluginHomeDirectory(const QString &dir) {m_pluginHomeDirectory = dir;}
private:
	QString m_featureId;
	QStringList m_dependsOnFeatureIds;
	QString m_pluginHomeDirectory;
};

}}}

#endif // QF_QMLWIDGETS_FRAMEWORK_PLUGINMANIFEST_H
