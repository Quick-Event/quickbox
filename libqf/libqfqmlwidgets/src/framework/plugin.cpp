#include "plugin.h"
#include "pluginmanifest.h"

#include <qf/core/utils.h>
#include <qf/core/log.h>

using namespace qf::qmlwidgets::framework;

Plugin::Plugin(QObject *parent) :
	QObject(parent), m_manifest(nullptr)
{
	qfLogFuncFrame();
}

Plugin::~Plugin()
{
	qfLogFuncFrame() << this;
}

void Plugin::setManifest(PluginManifest *mf)
{
	if(mf != m_manifest) {
		mf->setParent(this);
		m_manifest = mf;
		if(mf)
			setObjectName(mf->featureId());
		emit manifestChanged(mf);
	}
}
