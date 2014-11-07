#include "pluginmanifest.h"

using namespace qf::qmlwidgets::framework;

PluginManifest::PluginManifest(QObject *parent) :
	QObject(parent)
{
	qfLogFuncFrame();
}

PluginManifest::~PluginManifest()
{
	qfLogFuncFrame() << this;
}

void PluginManifest::setFeatureId(QString id)
{
	if(id != m_featureId) {
		m_featureId = id;
		setObjectName(id);
		emit featureIdChanged(id);
	}
}

void PluginManifest::setDependsOnFeatureIds(QStringList ids)
{
	m_dependsOnFeatureIds = ids;
}
