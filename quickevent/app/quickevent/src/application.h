#ifndef APPLICATION_H
#define APPLICATION_H

#include <qf/qmlwidgets/framework/application.h>

class AppCliOptions;
class TableModelLogDevice;

namespace qf { namespace core { class LogEntryMap; }}

class Application : public qf::qmlwidgets::framework::Application
{
	Q_OBJECT
private:
	typedef qf::qmlwidgets::framework::Application Super;
public:
	Application(int & argc, char ** argv, AppCliOptions *cli_opts);
	~Application() override = default;

	static Application* instance(bool must_exist = true);

	static int dbVersion();

	AppCliOptions* cliOptions() {return m_cliOptions;}

	Q_INVOKABLE QString versionString() const;
	Q_INVOKABLE QString dbVersionString() const;

	Q_SIGNAL void newLogEntry(const qf::core::LogEntryMap &le);
	void emitNewLogEntry(const qf::core::LogEntryMap &le) { emit newLogEntry(le); }
private:
	AppCliOptions *m_cliOptions;
};

#endif // APPLICATION_H
