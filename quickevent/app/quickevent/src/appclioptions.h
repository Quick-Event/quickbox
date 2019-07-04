#ifndef APPCLIOPTIONS_H
#define APPCLIOPTIONS_H

#include <qf/core/utils/clioptions.h>

#include <QUrl>

class AppCliOptions : public qf::core::utils::ConfigCLIOptions
{
	Q_OBJECT
private:
	typedef qf::core::utils::ConfigCLIOptions Super;
public:
	AppCliOptions(QObject *parent = NULL);
	~AppCliOptions() Q_DECL_OVERRIDE {}

	CLIOPTION_GETTER_SETTER2(QString, "server.host", s, setS, erverHost)
	CLIOPTION_GETTER_SETTER2(int, "server.port", s, setS, erverPort)
	CLIOPTION_GETTER_SETTER(QString, l, setL, ocale)
	CLIOPTION_GETTER_SETTER(QString, p, setP, rofile)
};

#endif // APPCLIOPTIONS_H
