#ifndef QF_CORE_NETWORK_NETWORKACCESSMANAGER_H
#define QF_CORE_NETWORK_NETWORKACCESSMANAGER_H

#include <QNetworkAccessManager>
//#include <QNetworkReply>

class QUrl;

namespace qf {
namespace core {
namespace qml {

class NetworkReply;

class NetworkAccessManager : public QNetworkAccessManager
{
	Q_OBJECT
private:
	typedef QNetworkAccessManager Super;
public:
	explicit NetworkAccessManager(QObject *parent = 0);
	~NetworkAccessManager() Q_DECL_OVERRIDE;
public:
	Q_SLOT qf::core::qml::NetworkReply* get(const QUrl &url);
private:
};

}}}

#endif // QF_CORE_NETWORK_NETWORKACCESSMANAGER_H
