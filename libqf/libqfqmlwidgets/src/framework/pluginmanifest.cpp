#include "pluginmanifest.h"

using namespace qf::qmlwidgets::framework;

PluginManifest::PluginManifest(QObject *parent) :
	QObject(parent)
{
}

PluginManifest::~PluginManifest()
{
}

void PluginManifest::setFeatureId(QString id)
{
	m_featureId = id;
}

void PluginManifest::setDependsOnFeatureIds(QStringList ids)
{
	m_dependsOnFeatureIds = ids;
}
