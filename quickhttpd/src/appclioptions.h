#ifndef APPCLIOPTIONS_H
#define APPCLIOPTIONS_H

#include <qf/core/utils/clioptions.h>

class AppCliOptions : public qf::core::utils::ConfigCLIOptions
{
	Q_OBJECT
private:
	typedef qf::core::utils::ConfigCLIOptions Super;
public:
	AppCliOptions(QObject *parent = NULL);
	~AppCliOptions() Q_DECL_OVERRIDE {}

	CLIOPTION_GETTER_SETTER2(QString, "application.htmlDir", h, setH, tmlDir)
	CLIOPTION_GETTER_SETTER2(int, "application.refreshTime", r, setR, efreshTime)
	CLIOPTION_GETTER_SETTER2(QString, "event.name", e, setE, ventName)
	CLIOPTION_GETTER_SETTER2(int, "event.stage", s, setS, tage)
	CLIOPTION_GETTER_SETTER2(QString, "event.classesLike", c, setC, lassesLike)
	CLIOPTION_GETTER_SETTER2(QString, "event.classesNotLike", c, setC, lassesNotLike)
	CLIOPTION_GETTER_SETTER2(QString, "sql.host", s, setS, qlHost)
	CLIOPTION_GETTER_SETTER2(int, "sql.port", s, setS, qlPort)
	CLIOPTION_GETTER_SETTER2(QString, "sql.user", s, setS, qlUser)
	CLIOPTION_GETTER_SETTER2(QString, "sql.password", s, setS, qlPassword)
	CLIOPTION_GETTER_SETTER2(QString, "sql.database", s, setS, qlDatabase)
	CLIOPTION_GETTER_SETTER2(QString, "sql.driver", s, setS, qlDriver)

	CLIOPTION_GETTER_SETTER2(int, "httpd.port", h, setH, ttpdPort)
};

#endif // APPCLIOPTIONS_H
