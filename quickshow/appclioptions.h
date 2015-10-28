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

	CLIOPTION_GETTER_SETTER(QString, p, setP, rofile)
	CLIOPTION_GETTER_SETTER2(int, "pplication.refreshTime", r, setr, efreshTime)
	CLIOPTION_GETTER_SETTER2(QString, "event.name", e, setE, ventName)
	CLIOPTION_GETTER_SETTER2(int, "event.stage", s, setS, tage)
	CLIOPTION_GETTER_SETTER2(QString, "connection.host", h, setH, ost)
	CLIOPTION_GETTER_SETTER2(int, "connection.port", p, setP, ort)
	CLIOPTION_GETTER_SETTER2(QString, "connection.user", u, setU, ser)
	CLIOPTION_GETTER_SETTER2(QString, "connection.password", p, setP, assword)
	CLIOPTION_GETTER_SETTER2(QString, "connection.database", d, setD, atabase)
	CLIOPTION_GETTER_SETTER2(QString, "connection.driver", s, setS, qlDriver)
};

#endif // APPCLIOPTIONS_H
