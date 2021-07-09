#ifndef APPCLIOPTIONS_H
#define APPCLIOPTIONS_H

#include <qf/core/utils/clioptions.h>

class AppCliOptions : public qf::core::utils::ConfigCLIOptions
{
	Q_OBJECT
private:
	typedef qf::core::utils::ConfigCLIOptions Super;
public:
	AppCliOptions(QObject *parent = nullptr);
	~AppCliOptions() Q_DECL_OVERRIDE {}

	CLIOPTION_GETTER_SETTER(QString, l, setL, ocale)
	CLIOPTION_GETTER_SETTER(QString, p, setP, rofile)
	CLIOPTION_GETTER_SETTER2(int, "application.refreshTime", r, setR, efreshTime)
	CLIOPTION_GETTER_SETTER2(int, "application.columnCount", c, setC, olumnCount)
	CLIOPTION_GETTER_SETTER2(int, "application.fontScale", f, setF, ontScale)
	CLIOPTION_GETTER_SETTER2(QString, "event.name", e, setE, ventName)
	CLIOPTION_GETTER_SETTER2(int, "event.stage", s, setS, tage)
	CLIOPTION_GETTER_SETTER2(QString, "event.classesLike", c, setC, lassesLike)
	CLIOPTION_GETTER_SETTER2(QString, "event.classesNotLike", c, setC, lassesNotLike)
	CLIOPTION_GETTER_SETTER2(QString, "event.classesIn", c, setC, lassesIn)
	CLIOPTION_GETTER_SETTER2(QString, "connection.host", h, setH, ost)
	CLIOPTION_GETTER_SETTER2(int, "connection.port", p, setP, ort)
	CLIOPTION_GETTER_SETTER2(QString, "connection.user", u, setU, ser)
	CLIOPTION_GETTER_SETTER2(QString, "connection.password", p, setP, assword)
	CLIOPTION_GETTER_SETTER2(QString, "connection.database", d, setD, atabase)
	CLIOPTION_GETTER_SETTER2(QString, "connection.driver", s, setS, qlDriver)
};

#endif // APPCLIOPTIONS_H
