#pragma once

#include <qf/qmlwidgets/framework/plugin.h>
//#include <qf/qmlwidgets/framework/ipersistentsettings.h>

class OrisImporter;
class TxtImporter;
class XmlImporter;

namespace Oris {

class OrisPlugin : public qf::qmlwidgets::framework::Plugin//, public qf::qmlwidgets::framework::IPersistentSettings
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
	XmlImporter *m_xmlImporter = nullptr;
};

}
