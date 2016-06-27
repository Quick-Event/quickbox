#ifndef APPLICATION_H
#define APPLICATION_H

#include <qf/qmlwidgets/framework/application.h>

class AppCliOptions;
class TableModelLogDevice;

class Application : public qf::qmlwidgets::framework::Application
{
	Q_OBJECT
private:
	typedef qf::qmlwidgets::framework::Application Super;
public:
	Application(int & argc, char ** argv, AppCliOptions *cli_opts);
	~Application() Q_DECL_OVERRIDE;

	static Application* instance(bool must_exist = true);

	AppCliOptions* cliOptions() {return m_cliOptions;}
private:
	AppCliOptions *m_cliOptions;
};

#endif // APPLICATION_H
