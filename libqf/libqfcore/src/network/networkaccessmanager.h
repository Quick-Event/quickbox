#ifndef QF_CORE_NETWORK_NETWORKACCESSMANAGER_H
#define QF_CORE_NETWORK_NETWORKACCESSMANAGER_H

#include "../core/coreglobal.h"

#include <QNetworkAccessManager>
//#include <QNetworkReply>

class QUrl;

namespace qf {
namespace core {
namespace network {

class NetworkReply;

class QFCORE_DECL_EXPORT NetworkAccessManager : public QNetworkAccessManager
{
	Q_OBJECT
public:
	explicit NetworkAccessManager(QObject *parent = 0);
public:
	Q_SLOT qf::core::network::NetworkReply* get(const QUrl &content_url);
};

}}}

#endif // QF_CORE_NETWORK_NETWORKACCESSMANAGER_H
