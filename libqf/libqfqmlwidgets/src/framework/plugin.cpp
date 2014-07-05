#include "plugin.h"
#include "pluginmanifest.h"

#include <qf/core/utils.h>

using namespace qf::qmlwidgets::framework;

Plugin::Plugin(QObject *parent) :
	QObject(parent), m_manifest(nullptr)
{
}

Plugin::~Plugin()
{
}

void Plugin::setManifest(PluginManifest *m)
{
	QF_SAFE_DELETE(m_manifest);
	m->setParent(this);
	m_manifest = m;
}
