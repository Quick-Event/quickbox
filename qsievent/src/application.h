#ifndef APPLICATION_H
#define APPLICATION_H

#include <qf/qmlwidgets/framework/application.h>

class Application : public qf::qmlwidgets::framework::Application
{
	Q_OBJECT
private:
	typedef qf::qmlwidgets::framework::Application Super;
public:
	Application(int & argc, char ** argv);
	~Application() Q_DECL_OVERRIDE;
};

#endif // APPLICATION_H
