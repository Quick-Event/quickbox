#include "networkaccessmanager.h"
#include "networkreply.h"

#include "../core/log.h"

#include <QUrl>
#include <QNetworkRequest>
#include <QNetworkReply>

namespace qf {
namespace core {
namespace network {

NetworkAccessManager::NetworkAccessManager(QObject *parent) :
	Super(parent)
{
}

NetworkAccessManager::~NetworkAccessManager()
{
	qfLogFuncFrame() << this;
}

NetworkReply *NetworkAccessManager::get(const QUrl &url)
{
	qfLogFuncFrame() << url.toString();
	NetworkReply *ret = new NetworkReply();
	QNetworkRequest rq(url);
	//rq.setRawHeader("User-Agent", "MyOwnBrowser 1.0");
	QNetworkReply *repl = Super::get(rq);
	ret->setReply(repl);
	return ret;
}

}}}
