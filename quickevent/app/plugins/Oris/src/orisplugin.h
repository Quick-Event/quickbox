#pragma once

#include "orispluginglobal.h"

#include <qf/qmlwidgets/framework/plugin.h>
//#include <qf/qmlwidgets/framework/ipersistentsettings.h>

class OrisImporter;
class TxtImporter;

namespace Oris {

class ORISPLUGIN_DECL_EXPORT OrisPlugin : public qf::qmlwidgets::framework::Plugin//, public qf::qmlwidgets::framework::IPersistentSettings
{
	Q_OBJECT
	using Super = qf::qmlwidgets::framework::Plugin;
public:
	OrisPlugin(QObject *parent = nullptr);
private:
	void onInstalled();
private:
	OrisImporter *m_orisImporter = nullptr;
	TxtImporter *m_txtImporter = nullptr;
};

}
